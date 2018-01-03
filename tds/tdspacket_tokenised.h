#ifndef _TDSPACKET_TOKENISED_H
#define _TDSPACKET_TOKENISED_H

#include "../tdspsql.h"

class tdstoken;

class tdspacket_tokenised : public tdspacket {
private:
  tdstoken *token_head;
  tdstoken *token_tail;

protected:
  //TDS_MESSAGE_TYPE get_packet_type() { return TDS_MESSAGE_TYPE_RESPONSE; }
  int writepacket(dynbuffer *buffer);

public:
  using tdspacket::writepacket;
  tdspacket_tokenised(uint32_t tdsversion);
  void add_token(tdstoken *token);
};

#endif
