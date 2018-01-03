#ifndef _TDSLOGINPACKET_H
#define _TDSLOGINPACKET_H

/* http://msdn.microsoft.com/en-us/library/dd304019.aspx */

#include "../tdspsql.h"

class tdspacket_login : public tdspacket {
private:

protected:
  tdspacket_login(uint8_t packet_type, char *buffer, size_t length, uint32_t tdsversion);
  friend tdspacket *tdspacket::readpacket(int sock, uint32_t tdsversion);
  uint32_t login_length;
  uint32_t login_tdsversion;
  uint32_t login_packetsize;
  uint8_t login_flags1;
  uint8_t login_flags2;
  uint8_t login_typeflags;
  uint8_t login_extraflags;
  char *login_hostname;
  char *login_username;
  char *login_password;
  char *login_database;

public:
  tdspacket_login(uint32_t tdsversion);
  uint32_t get_login_length() { return login_length; }
  uint32_t get_login_tdsversion() { return login_tdsversion; }
  uint32_t get_login_packetsize() { return login_packetsize; }
  uint8_t get_login_flags1() { return login_flags1; }
  uint8_t get_login_flags2() { return login_flags2; }
  uint8_t get_login_typeflags() { return login_typeflags; }
  uint8_t get_login_extraflags() { return login_extraflags; }
  char *get_login_hostname() { return login_hostname; }
  char *get_login_username() { return login_username; }
  char *get_login_password() { return login_password; }
  char *get_login_database() { return login_database; }
};

#endif

