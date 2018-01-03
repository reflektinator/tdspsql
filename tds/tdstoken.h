#ifndef _TDSTOKEN_H
#define _TDSTOKEN_H

#include "../tdspsql.h"

class tdspacket_tokenised;

class tdstoken {
private:
  tdstoken *next;
  tdstoken();

protected:
  uint8_t token_type;

  friend class tdspacket_tokenised;

  tdstoken(uint8_t token_type);
  virtual ~tdstoken() {};
  virtual size_t writetoken(dynbuffer *bufffer, uint32_t tdsversion);

public:
  uint8_t get_type() { return token_type; }
};

#endif
