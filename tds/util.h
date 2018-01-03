#ifndef UTIL_H
#define UTIL_H

#include "../tdspsql.h"

#define STATE_FAIL -1
#define STATE_PRELOGIN 0
#define STATE_LOGIN 1
#define STATE_CONNECTED 2

/* network order */
typedef struct {
  uint8_t message_type;
  uint8_t message_status;
  uint16_t message_length; /* including this header */
  uint16_t message_spid;
  uint8_t message_packetid;
  uint8_t message_windows;
} TDS_MESSAGE_HEADER;

static inline uint8_t GET_U8(char *buffer, uint32_t offset) { return (uint8_t)(buffer[offset]); }

#define GET_NU16(buffer, offset) ntohs(*(uint16_t *)&((buffer)[offset]))
#define GET_HU16(buffer, offset) (*(uint16_t *)&((buffer)[offset]))

#define GET_NU32(buffer, offset) ntohl(*(uint32_t *)&((buffer)[offset]))
#define GET_HU32(buffer, offset) (*(uint32_t *)&((buffer)[offset]))

/* length is in bytes */
static inline char *GET_STRING_UCS2(char *buffer, uint32_t offset, size_t length, int is_password) {
  iconv_t cd;
  char *inbufptr;
  char *outbuf;
  char *outbufptr;
  size_t inbytesleft;
  size_t outbytesleft;
  char *tmp = NULL;

  inbufptr = buffer + offset;
  inbytesleft = length;
  if (is_password) {
    tmp = new char[length];
    if (!tmp) {
      return NULL;
    }
    for (int j = 0; j < (int)length; j++) {
      tmp[j] = inbufptr[j] ^ 0xA5;
      tmp[j] = ((tmp[j] & 0x0F) << 4) | ((tmp[j] & 0xF0) >> 4);
    }
    inbufptr = tmp;
  }
  cd = iconv_open("UTF-8//IGNORE", "UCS-2//IGNORE");
  if (cd == (iconv_t)-1) {
    return NULL;
  }
  int allocsize = length / 2 + 1;
  outbuf = new char[allocsize];
  outbufptr = outbuf;
  outbytesleft = allocsize - 1;
  int charsize = 1;
  /* allocate an increasing amount of space for the remaining characters if we run out */
  while (inbytesleft) {
    if (iconv(cd, &inbufptr, &inbytesleft, &outbufptr, &outbytesleft) == (size_t)-1) {
      delete outbuf;
      if (tmp) {
        delete tmp;
      }
      iconv_close(cd);
      return NULL;
    }
    if (inbytesleft) {
      charsize++;
      printf("reallocating with charsize = %d\n", charsize);
      char *outbufnew = new char[allocsize + inbytesleft * charsize + 1];
      if (!outbufnew) {
        delete outbuf;
        if (tmp) {
          delete tmp;
        }
        iconv_close(cd);
        return NULL;
      }
      memcpy(outbufnew, outbuf, allocsize);
      allocsize += inbytesleft * 2 + 1;
      outbufptr += outbufnew - outbuf;
      delete outbuf;
      outbuf = outbufnew;
    }
  }
  *outbufptr = 0;
  iconv_close(cd);
  if (tmp) {
    delete tmp;
  }
  return outbuf;
}

/* should merge this with the above */
static inline char *GET_STRING_H16_UCS2(char *buffer, uint32_t offset, int is_password) {
  iconv_t cd;
  uint16_t field_offset;
  uint16_t field_length;
  char *inbufptr;
  char *outbuf;
  char *outbufptr;
  size_t inbytesleft;
  size_t outbytesleft;
  char *tmp = NULL;
  field_offset = GET_HU16(buffer, offset + 0);
  /* field_length is in UTF16 chars (always?) */
  field_length = GET_HU16(buffer, offset + 2);
  if (!field_length) {
    outbuf = new char[1];
    outbuf[0] = 0;
    return outbuf;
  }
  inbufptr = buffer + field_offset;
  inbytesleft = field_length * 2;
  if (is_password) {
    tmp = new char[field_length * 2];
    if (!tmp) {
      return NULL;
    }
    for (int j = 0; j < field_length * 2; j++) {
      tmp[j] = inbufptr[j] ^ 0xA5;
      tmp[j] = ((tmp[j] & 0x0F) << 4) | ((tmp[j] & 0xF0) >> 4);
    }
    inbufptr = tmp;
  }
  cd = iconv_open("UTF-8//IGNORE", "UCS-2//IGNORE");
  if (cd == (iconv_t)-1) {
    return NULL;
  }
  int allocsize = field_length + 1;
  outbuf = new char[allocsize];
  outbufptr = outbuf;
  outbytesleft = allocsize - 1;
  int charsize = 1;
  /* allocate an increasing amount of space for the remaining characters if we run out */
  while (inbytesleft) {
    if (iconv(cd, &inbufptr, &inbytesleft, &outbufptr, &outbytesleft) == (size_t)-1) {
      delete outbuf;
      if (tmp) {
        delete tmp;
      }
      iconv_close(cd);
      return NULL;
    }
    if (inbytesleft) {
      charsize++;
      printf("reallocating with charsize = %d\n", charsize);
      char *outbufnew = new char[allocsize + inbytesleft * charsize + 1];
      if (!outbufnew) {
        delete outbuf;
        if (tmp) {
          delete tmp;
        }
        iconv_close(cd);
        return NULL;
      }
      memcpy(outbufnew, outbuf, allocsize);
      allocsize += inbytesleft * 2 + 1;
      outbufptr += outbufnew - outbuf;
      delete outbuf;
      outbuf = outbufnew;
    }
  }
  *outbufptr = 0;
  iconv_close(cd);
  if (tmp) {
    delete tmp;
  }
  return outbuf;
}

static inline void PUT_U8(char *buffer, int offset, uint8_t value) { *(uint8_t *)(buffer + offset) = value; }
static inline void PUT_NU16(char *buffer, int offset, uint16_t value) { *(uint16_t *)(buffer + offset) = htons(value); }
static inline void PUT_NU32(char *buffer, int offset, uint32_t value) { *(uint32_t *)(buffer + offset) = htonl(value); }

static inline void PUT_HU16(char *buffer, int offset, uint16_t value) { *(uint16_t *)(buffer + offset) = value; }
static inline void PUT_HU32(char *buffer, int offset, uint32_t value) { *(uint32_t *)(buffer + offset) = value; }

//#define PUT_U8(buffer, offset, value) (buffer)[offset] = value
//#define PUT_NU16(buffer, offset, value) (*(uint16_t *)&(buffer)[offset]) = htons(value)

#endif

