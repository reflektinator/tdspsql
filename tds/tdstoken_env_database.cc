#include "../tdspsql.h"

tdstoken_env_database::tdstoken_env_database() : tdstoken(0xE3) {
  old_database = "";
  new_database = "";
};

size_t
tdstoken_env_database::writetoken(dynbuffer *buffer, uint32_t tdsversion) {
  tdstoken::writetoken(buffer, tdsversion);
  buffer->add_length_hu16(-3);
  buffer->add_u8(1); // database
  buffer->add_cc1_ucs2(new_database.c_str());
  buffer->add_cc1_ucs2(old_database.c_str());
  return 0;
}

