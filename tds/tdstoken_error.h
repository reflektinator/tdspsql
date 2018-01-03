#ifndef _TDSTOKEN_ERROR_H
#define _TDSTOKEN_ERROR_H

#include "../tdspsql.h"

class tdstoken_error : public tdstoken {
private:
  uint32_t error_number;
  uint8_t error_state;
  uint8_t error_severity;
  uint32_t error_line_number; /* 16 bits for < 7.2 */
  char *error_message;

protected:

public:
  tdstoken_error();
  size_t writetoken(dynbuffer *buffer, uint32_t tdsversion);

  void set_error_number(uint32_t value) { error_number = value; }
  void set_error_state(uint8_t value) { error_state = value; }
  void set_error_severity(uint8_t value) { error_severity = value; }
  void set_error_line_number(uint32_t value) { error_line_number = value; }
  void set_error_message(char *value) { error_message = value; }
};

#endif

