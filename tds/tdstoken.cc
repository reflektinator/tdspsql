#include "../tdspsql.h"

tdstoken::tdstoken(uint8_t token_type) {
  this->token_type = token_type;
  next = NULL;
};

size_t
tdstoken::writetoken(dynbuffer *buffer, uint32_t tdsversion) {
  buffer->add_u8(token_type);
  return 1;
}
