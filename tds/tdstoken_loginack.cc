#include "../tdspsql.h"

/* version numbers http://msdn.microsoft.com/en-us/library/dd339982.aspx */

size_t
tdstoken_loginack::writetoken(dynbuffer *buffer, uint32_t tdsversion) {
  tdstoken::writetoken(buffer, tdsversion);
  buffer->add_length_hu16(-3);
  buffer->add_u8(token_interface);
  buffer->add_nu32(token_tdsversion);
  buffer->add_cc1_ucs2(token_serverproduct);
  buffer->add_u8(10); /* SQL 2008R2 */
  buffer->add_u8(50);
  buffer->add_u8(0);
  buffer->add_u8(0);
  return 0;
}
