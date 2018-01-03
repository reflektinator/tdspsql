#ifndef _TDSTOKEN_INFO_H
#define _TDSTOKEN_INFO_H

#include "../tdspsql.h"

class tdstoken_info : public tdstoken {
private:
  uint32_t info_number;
  uint8_t info_state;
  uint8_t info_severity;
  uint32_t info_line_number; /* 16 bits for < 7.2 */
  char *info_message;

protected:

public:
  tdstoken_info();
  size_t writetoken(dynbuffer *buffer, uint32_t tdsversion);

  void set_info_number(uint32_t value) { info_number = value; }
  void set_info_state(uint8_t value) { info_state = value; }
  void set_info_severity(uint8_t value) { info_severity = value; }
  void set_info_line_number(uint32_t value) { info_line_number = value; }
  void set_info_message(char *value) { info_message = value; }
};

#endif

