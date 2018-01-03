#ifndef _TDSTOKEN_DONE_H
#define _TDSTOKEN_DONE_H

#include "../tdspsql.h"

class tdstoken_done : public tdstoken {
private:
  union {
    struct {
      uint16_t more_flag:1;
      uint16_t error_flag:1;
      uint16_t inxact_flag:1; /* transaction in progress */
      uint16_t reserved1:1;
      uint16_t count_flag:1; /* count is valid */
      uint16_t attn_flag:1; /* ack of client attention */
      uint16_t srverror_flag:1;
    };
    uint16_t word;
  } token_status;
  uint16_t token_curcmd;
  uint64_t token_rowcount; /* 32 bits when version < 7.2 */

protected:

public:
  tdstoken_done();
  size_t writetoken(dynbuffer *buffer, uint32_t tdsversion);

  void set_status_more(uint16_t value) { token_status.more_flag = value; }
  void set_status_error(uint16_t value) { token_status.error_flag = value; token_rowcount = value?1:token_rowcount; } /* seems we need rowcount = 1 */
  void set_status_word(uint16_t value) { token_status.word = value; }
  void set_curcmd(uint16_t value) { token_curcmd = value; }
  void set_rowcount(uint64_t value) { token_rowcount = value; token_status.count_flag = 1; }
};

#endif

