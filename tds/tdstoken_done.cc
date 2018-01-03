#include "../tdspsql.h"

tdstoken_done::tdstoken_done() : tdstoken(0xFD) {
  token_status.word = 0;
  token_curcmd = 0;
  token_rowcount = 0;
};

size_t
tdstoken_done::writetoken(dynbuffer *buffer, uint32_t tdsversion) {
  tdstoken::writetoken(buffer, tdsversion);
  buffer->add_hu16(token_status.word);
  buffer->add_hu16(token_curcmd);
  buffer->add_hu32((uint32_t)token_rowcount & 0xFFFFFFFF);
  /* can be 64 bit value in some cases */
  if (tdsversion >= 0x72000000) {
    buffer->add_nu32((uint32_t)(token_rowcount > 32));
  }
  return 0;
}

