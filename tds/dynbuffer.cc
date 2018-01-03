#include "../tdspsql.h"

void
dynbuffer::init() {
  parent = NULL;
  buffer_pos = 0;
  length_type = DYNBUFFER_LENGTH_TYPE_NONE;
  tailbuffer_head = NULL;
  finalised = 0;
}

dynbuffer::dynbuffer() {
  init();
  buffer_size = 1024;
  buffer = new char[buffer_size];
}

dynbuffer::dynbuffer(size_t initial_size) {
  init();
  buffer_size = initial_size;
  buffer = new char[buffer_size];
}

dynbuffer::dynbuffer(dynbuffer *parent) {
  init();
  this->parent = parent;
  buffer = parent->buffer + parent->buffer_pos;
  buffer_size = 0; /* make_room calls parent */
}

void
dynbuffer::finalise() {
  tailbuffer *tailbuffer;

  if (finalised) {
    return;
  }
  finalised = 1;
  for (tailbuffer = tailbuffer_head; tailbuffer; tailbuffer = tailbuffer->next) {
    make_room(tailbuffer->buffer_size);
    switch(tailbuffer->pointer_type) {
    case DYNBUFFER_POINTER_TYPE_NU16:
      PUT_NU16(buffer, tailbuffer->pointer_pos, buffer_pos + tailbuffer->pointer_offset);
      memcpy(buffer + buffer_pos, tailbuffer->buffer, tailbuffer->buffer_size);
      buffer_pos += tailbuffer->buffer_size;
      break;
    default:
      printf("Unknown length type %d\n", length_type);
      break;
    }
  }
  switch(length_type) {
  case DYNBUFFER_LENGTH_TYPE_NONE:
    break;
  case DYNBUFFER_LENGTH_TYPE_NU16:
    PUT_NU16(buffer, length_pos, buffer_pos + length_offset);
    break;
  case DYNBUFFER_LENGTH_TYPE_HU16:
    PUT_HU16(buffer, length_pos, buffer_pos + length_offset);
    break;
  default:
    printf("Unknown length type %d\n", length_type);
    break;
  }
  if (parent) {
    parent->buffer_pos += buffer_pos;
  }
}

int dynbuffer::write(int sock) {
  finalise();
  //return ::write(sock, buffer, buffer_pos);
  return ::send(sock, buffer, buffer_pos, MSG_MORE);
}

int dynbuffer::write(int sock, int offset, int length) {
  finalise();
  return ::write(sock, buffer + offset, length);
}

int
dynbuffer::make_room(size_t size) {
  if (parent) {
    parent->make_room(buffer_pos + size);
    buffer = parent->buffer + parent->buffer_pos;
    return 1;
  }
  if (buffer_pos + size > buffer_size) {
    char *new_buffer = new char[buffer_size * 2];
    printf("new buffer %p allocated %d > %d\n", new_buffer, (int)buffer_size + (int)size, (int)buffer_pos);
    /* have to copy the whole buffer because the child buffer doesn't update buffer_pos */
    memcpy(new_buffer, buffer, buffer_size);
    buffer_size *= 2;
    delete buffer;
    buffer = new_buffer;
  }
  return 1;
}

int
dynbuffer::add_bytes(const char *bytes, size_t size) {
  make_room(size);
  memcpy(buffer + buffer_pos, bytes, size);
  buffer_pos += size;
  return 1;
}

int
dynbuffer::add_bytes(const char *bytes, size_t size, int pointer_type, int pointer_offset) {
  tailbuffer *tb;

  switch(pointer_type) {
  case DYNBUFFER_POINTER_TYPE_NU16:
    make_room(sizeof(uint16_t));
    break;
  default:
    printf("Unknown length type %d\n", length_type);
    return 0;

  }
  tb = new tailbuffer();
  tb->buffer = new char[size];
  memcpy(tb->buffer, bytes, size);
  tb->buffer_size = size;
  tb->pointer_type = pointer_type;
  tb->pointer_pos = buffer_pos;
  tb->pointer_offset = pointer_offset;
  add_nu16(0xFFFF); /* fill in on finalise */
  if (!tailbuffer_head) {
    tailbuffer_head = tb;
    tailbuffer_tail = tb;
  } else {
    tailbuffer_tail->next = tb;
    tailbuffer_tail = tb;
  }
  return 1;
}

int dynbuffer::add_length_nu16(int offset) {
  length_type = DYNBUFFER_LENGTH_TYPE_NU16;
  length_pos = buffer_pos;
  length_offset = offset;
  add_nu16(0xFFFF);
  return 1;
}

int dynbuffer::add_length_hu16(int offset) {
  length_type = DYNBUFFER_LENGTH_TYPE_HU16;
  length_pos = buffer_pos;
  length_offset = offset;
  add_nu16(0xFFFF);
  return 1;
}

int dynbuffer::add_u8(uint8_t value) {
  make_room(sizeof(value));
  PUT_U8(buffer, buffer_pos, value);
  buffer_pos += sizeof(value);
  return 1;
}

int dynbuffer::add_nu16(uint16_t value) {
  make_room(sizeof(value));
  PUT_NU16(buffer, buffer_pos, value);
  buffer_pos += sizeof(value);
  return 1;
}

int dynbuffer::add_hu16(uint16_t value) {
  make_room(sizeof(value));
  PUT_HU16(buffer, buffer_pos, value);
  buffer_pos += sizeof(value);
  return 1;
}

int dynbuffer::add_nu32(uint32_t value) {
  make_room(sizeof(value));
  PUT_NU32(buffer, buffer_pos, value);
  buffer_pos += sizeof(value);
  return 1;
}

int dynbuffer::add_hu32(uint32_t value) {
  make_room(sizeof(value));
  PUT_HU32(buffer, buffer_pos, value);
  buffer_pos += sizeof(value);
  return 1;
}

int
dynbuffer::add_cch2_utf8(const char *value) {
  make_room(2 + strlen(value));
  PUT_HU16(buffer, buffer_pos, strlen(value));
  buffer_pos += 2;
  strcpy(buffer + buffer_pos, value);
  buffer_pos += strlen(value);
  return 1;
}

int
dynbuffer::add_cch4_utf8(const char *value) {
  make_room(4 + strlen(value));
  PUT_HU32(buffer, buffer_pos, strlen(value));
  buffer_pos += 4;
  strcpy(buffer + buffer_pos, value);
  buffer_pos += strlen(value);
  return 1;
}

int
dynbuffer::add_cc1_ucs2(const char *value) {
  size_t inbytesleft;
  size_t outbytesleft;
  const char *inbufptr;
  char *outbufptr;
  iconv_t cd;
  int factor = 2;
  size_t outbytes_count;
  size_t cc_pos;

  cd = iconv_open("UCS-2//TRANSLIT", "UTF-8//TRANSLIT");
  if (cd == (iconv_t)-1) {
    perror("iconv_open");
    return 0;
  }

  cc_pos = buffer_pos;
  make_room(1);
  buffer_pos++;
  outbytes_count = 0;
  inbytesleft = strlen(value);
  inbufptr = value;
  while(inbytesleft) {
    size_t res;
    size_t outbytes;
    outbytes = inbytesleft * factor;
    make_room(outbytes);
    outbytesleft = outbytes;
    //printf("buffer_size = %d, buffer_pos = %d\n", (int)buffer_size, (int)buffer_pos);
    //printf("converting %s, inbytesleft = %d, outbytesleft = %d\n", inbufptr, (int)inbytesleft, (int)outbytesleft);
    outbufptr = buffer + buffer_pos;
    res = iconv(cd, (char **)&inbufptr, &inbytesleft, &outbufptr, &outbytesleft);
    if (res == (size_t)-1) {
      perror("iconv");
    }
    //printf("res = %ld\n", res);
    outbytes_count += outbytes - outbytesleft; 
    buffer_pos += outbytes - outbytesleft; 
  }
  PUT_U8(buffer, cc_pos, outbytes_count / 2);
  return 1;
}

int
dynbuffer::add_cch2_ucs2(const char *value) {
  size_t inbytesleft;
  size_t outbytesleft;
  const char *inbufptr;
  char *outbufptr;
  iconv_t cd;
  int factor = 2;
  size_t outbytes_count;
  size_t cc_pos;

  cd = iconv_open("UCS-2//TRANSLIT", "UTF-8//TRANSLIT");
  if (cd == (iconv_t)-1) {
    perror("iconv_open");
    return 0;
  }

  cc_pos = buffer_pos;
  make_room(1);
  buffer_pos+=2;
  outbytes_count = 0;
  inbytesleft = strlen(value);
  inbufptr = value;
  while(inbytesleft) {
    size_t res;
    size_t outbytes;
    outbytes = inbytesleft * factor;
    make_room(outbytes);
    outbytesleft = outbytes;
    //printf("buffer_size = %d, buffer_pos = %d\n", (int)buffer_size, (int)buffer_pos);
    //printf("converting %s, inbytesleft = %d, outbytesleft = %d\n", inbufptr, (int)inbytesleft, (int)outbytesleft);
    outbufptr = buffer + buffer_pos;
    res = iconv(cd, (char **)&inbufptr, &inbytesleft, &outbufptr, &outbytesleft);
    if (res == (size_t)-1) {
      perror("iconv");
    }
    //printf("res = %ld\n", res);
    outbytes_count += outbytes - outbytesleft; 
    buffer_pos += outbytes - outbytesleft; 
  }
  PUT_HU16(buffer, cc_pos, outbytes_count / 2);
  return 1;
}


