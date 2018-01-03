#include "../tdspsql.h"

tdstoken_error::tdstoken_error() : tdstoken(0xAA) {
  error_number = 0;
  error_state = 0;
  error_severity = 0;
  error_line_number = 1;
  error_message = NULL;
};

size_t
tdstoken_error::writetoken(dynbuffer *buffer, uint32_t tdsversion) {
  tdstoken::writetoken(buffer, tdsversion);
  buffer->add_length_hu16(-3);
  buffer->add_hu32(error_number);
  buffer->add_u8(error_state);
  buffer->add_u8(error_severity);
  buffer->add_cch2_ucs2(error_message);
  buffer->add_cc1_ucs2(""); /* server name */
  buffer->add_cc1_ucs2(""); /* process name */
  if (tdsversion >= 0x72000000) {
    buffer->add_hu32(error_line_number);
  } else {
    buffer->add_hu16(error_line_number);
  }
  return 0;
}

