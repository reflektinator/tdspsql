#include "../tdspsql.h"

tdstoken_env_collation::tdstoken_env_collation() : tdstoken(0xE3) {
};

size_t
tdstoken_env_collation::writetoken(dynbuffer *buffer, uint32_t tdsversion) {
  tdstoken::writetoken(buffer, tdsversion);
  buffer->add_length_hu16(-3);
  buffer->add_u8(7); // collation
  buffer->add_u8(5); // new value length
  buffer->add_hu16(1033);
  buffer->add_hu16(0x00D0);
  buffer->add_u8(0x00);
  buffer->add_u8(0); // old value length
  return 0;
}

