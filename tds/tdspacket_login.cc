#include "../tdspsql.h"

tdspacket_login::tdspacket_login(uint32_t tdsversion) : tdspacket(tdsversion) {
  // zero fields
  // set some reasonable defaults
  //set_length(get_length() + 0);
}

tdspacket_login::tdspacket_login(uint8_t packet_type, char *buffer, size_t length, uint32_t tdsversion) : tdspacket(packet_type, buffer, length, tdsversion) {
  switch (packet_type) {
  case TDS_MESSAGE_TYPE_LOGIN4:
    login_length = length;
    login_tdsversion = GET_HU32(buffer, 458);
    login_packetsize = 4096; //GET_HU32(buffer, 8);
    login_flags1 = 0; //GET_U8(buffer, 24);
    login_flags2 = 0; //GET_U8(buffer, 25);
    login_typeflags = 0; //GET_U8(buffer, 26);
    login_extraflags = 0; //GET_U8(buffer, 27);
    login_hostname = strndup(buffer + 0, buffer[30]);
    login_username = strndup(buffer + 31, buffer[61]);
    login_password = strndup(buffer + 62, buffer[92]);
    login_database = strdup("");
    break;
  case TDS_MESSAGE_TYPE_LOGIN7:
    login_length = GET_HU32(buffer, 0);
    login_tdsversion = GET_HU32(buffer, 4);
    login_packetsize = GET_HU32(buffer, 8);
    login_flags1 = GET_U8(buffer, 24);
    login_flags2 = GET_U8(buffer, 25);
    login_typeflags = GET_U8(buffer, 26);
    login_extraflags = GET_U8(buffer, 27);
    login_hostname = GET_STRING_H16_UCS2(buffer, 36, 0);
    login_username = GET_STRING_H16_UCS2(buffer, 40, 0);
    login_password = GET_STRING_H16_UCS2(buffer, 44, 1);
    login_database = GET_STRING_H16_UCS2(buffer, 68, 0);
    break;
  default:
    // wtf?
    printf("????\n");
  }
}
