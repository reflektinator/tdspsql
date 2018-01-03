#include "../tdspsql.h"

tdspacket_sql::tdspacket_sql(uint8_t packet_type, char *buffer, size_t length, uint32_t tdsversion) : tdspacket(packet_type, buffer, length, tdsversion) {
  uint32_t header_length = 0;
  if (tdsversion >= 0x72000000) {
    header_length = GET_HU32(buffer, 0);
  }
  sql_query = GET_STRING_UCS2(buffer, header_length, message_length, 0);
}
