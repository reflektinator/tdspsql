#include "../tdspsql.h"

tdstoken_info::tdstoken_info() : tdstoken(0xAB) {
  info_number = 0;
  info_state = 0;
  info_severity = 0;
  info_line_number = 1;
  info_message = NULL;
};

size_t
tdstoken_info::writetoken(dynbuffer *buffer, uint32_t tdsversion) {
  tdstoken::writetoken(buffer, tdsversion);
  buffer->add_length_hu16(-3);
  buffer->add_hu32(info_number);
  buffer->add_u8(info_state);
  buffer->add_u8(info_severity);
  buffer->add_cch2_ucs2(info_message);
  buffer->add_cc1_ucs2(""); /* server name */
  buffer->add_cc1_ucs2(""); /* process name */
  if (tdsversion >= 0x72000000) {
    buffer->add_hu32(info_line_number);
  } else {
    buffer->add_hu16(info_line_number);
  }
  return 0;
}

