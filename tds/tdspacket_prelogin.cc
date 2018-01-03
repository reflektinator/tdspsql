#include "../tdspsql.h"

tdspacket_prelogin::tdspacket_prelogin(uint32_t tdsversion) : tdspacket(tdsversion) {
  packet_type = TDS_MESSAGE_TYPE_RESPONSE;
//  packet_type = TDS_MESSAGE_TYPE_RESPONSE; /* PRELOGIN responses are type RESPONSE */
//  packet_status.eom_flag = 1;
}

tdspacket_prelogin::tdspacket_prelogin(uint8_t packet_type, char *buffer, size_t total_length, uint32_t tdsversion) : tdspacket(packet_type, buffer, total_length, tdsversion) {
  int i;
  uint8_t id;
  uint16_t offset;
  //uint16_t length;

  token_bitfield = 0;
  for (i = 0; i < 32 && (id = GET_U8(buffer, i * 5)) != 0xFF; i++) {
    token_bitfield |= (id << i); /* check bounds? */
    offset = GET_NU16(buffer, i * 5 + 1);
    switch (id) {
    case PRELOGIN_TOKEN_VERSION:
      token_version = *(PRELOGIN_VERSION *)(buffer + offset);
      break;
    case PRELOGIN_TOKEN_ENCRYPTION:
      token_encryption = GET_U8(buffer, offset);
      break;
    case PRELOGIN_TOKEN_INSTANCE:
      token_instance = GET_STRING_H16_UCS2(buffer, offset, 0);
      break;
    case PRELOGIN_TOKEN_THREADID: // threadid
      token_threadid = GET_HU32(buffer, offset);
      break;
    case PRELOGIN_TOKEN_MARS: // mars
      token_mars = GET_U8(buffer, offset);
      break;
    default:
      printf("unknown option %d\n", id);
      break;
    }
  }
}

int
tdspacket_prelogin::writepacket(dynbuffer *buffer) {
  //buffer->add_u8(TDS_MESSAGE_TYPE_RESPONSE);
  //tdspacket::writepacket(buffer);
  if (token_bitfield & (1 << PRELOGIN_TOKEN_VERSION)) {
    buffer->add_u8(PRELOGIN_TOKEN_VERSION);
    buffer->add_bytes((char *)&token_version, sizeof(token_version), DYNBUFFER_POINTER_TYPE_NU16, 0);
    buffer->add_nu16(sizeof(token_version));
  }
  if (token_bitfield & (1 << PRELOGIN_TOKEN_ENCRYPTION)) {
    buffer->add_u8(PRELOGIN_TOKEN_ENCRYPTION);
    buffer->add_bytes((char *)&token_encryption, sizeof(token_encryption), DYNBUFFER_POINTER_TYPE_NU16, 0);
    buffer->add_nu16(sizeof(token_encryption));
  }
  if (token_bitfield & (1 << PRELOGIN_TOKEN_INSTANCE)) {
    buffer->add_u8(PRELOGIN_TOKEN_INSTANCE);
    buffer->add_bytes(token_instance, 1, DYNBUFFER_POINTER_TYPE_NU16, 0);
    buffer->add_nu16(1);
  }
  if (token_bitfield & (1 << PRELOGIN_TOKEN_THREADID)) {
    buffer->add_u8(PRELOGIN_TOKEN_THREADID);
    buffer->add_bytes((char *)&token_threadid, 0, DYNBUFFER_POINTER_TYPE_NU16, 0);
    buffer->add_nu16(0);
  }
  if (token_bitfield & (1 << PRELOGIN_TOKEN_MARS)) {
    buffer->add_u8(PRELOGIN_TOKEN_MARS);
    buffer->add_bytes((char *)&token_mars, 1, DYNBUFFER_POINTER_TYPE_NU16, 0);
    buffer->add_nu16(1);
  }
  buffer->add_u8(PRELOGIN_TOKEN_TERMINATOR);
  return 1;
}

char *
tdspacket_prelogin::get_instance() {
  if (!(token_bitfield & (1 << PRELOGIN_TOKEN_INSTANCE))) {
    return NULL;
  }
  return strdup(token_instance);
}

void
tdspacket_prelogin::set_version(PRELOGIN_VERSION value) {
  token_bitfield |= (1 << PRELOGIN_TOKEN_VERSION);
  token_version = value;
}

void
tdspacket_prelogin::set_encryption(uint8_t value) {
  token_bitfield |= (1 << PRELOGIN_TOKEN_ENCRYPTION);
  token_encryption = value;
}

void
tdspacket_prelogin::set_instance(uint8_t value) {
  token_bitfield |= (1 << PRELOGIN_TOKEN_INSTANCE);
  token_instance = new char[2];
  token_instance[0] = value;
  token_instance[1] = 0;
}

void
tdspacket_prelogin::set_threadid(uint32_t value) {
  token_bitfield |= (1 << PRELOGIN_TOKEN_THREADID);
  token_threadid = value;
}

void
tdspacket_prelogin::set_mars(uint8_t value) {
  token_bitfield |= (1 << PRELOGIN_TOKEN_MARS);
  token_mars = value;
}
