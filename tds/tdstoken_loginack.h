#ifndef _TDSTOKEN_LOGINACK_H
#define _TDSTOKEN_LOGINACK_H

#include "../tdspsql.h"

class tdstoken_loginack : public tdstoken {
private:

protected:
  uint16_t token_length;
  uint8_t token_interface;
  uint32_t token_tdsversion;
  char *token_serverproduct;
  uint8_t token_majorver;
  uint8_t token_minorver;
  uint8_t token_buildnumhi;
  uint8_t token_buildnumlow;

public:
  void set_tdsversion(uint32_t value) { token_tdsversion = value; }
  void set_interface(uint8_t value) { token_interface = value; }
  void set_serverproduct(const char *value) {token_serverproduct = strdup(value); }

  tdstoken_loginack() : tdstoken(0xAD) { };
  size_t writetoken(dynbuffer *buffer, uint32_t tdsversion);
};

#endif

