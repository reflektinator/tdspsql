#ifndef _TDSPRELOGINPACKET_H
#define _TDSPRELOGINPACKET_H

#include "../tdspsql.h"

#define PRELOGIN_TOKEN_VERSION    0x00
#define PRELOGIN_TOKEN_ENCRYPTION 0x01
#define PRELOGIN_TOKEN_INSTANCE   0x02
#define PRELOGIN_TOKEN_THREADID   0x03
#define PRELOGIN_TOKEN_MARS       0x04
#define PRELOGIN_TOKEN_TERMINATOR 0xFF

#define TDS_PRELOGIN_ENCRYPT_OFF      0x00
#define TDS_PRELOGIN_ENCRYPT_ON       0x01
#define TDS_PRELOGIN_ENCRYPT_NOT_SUPP 0x02
#define TDS_PRELOGIN_ENCRYPT_REQ      0x03

#pragma pack(1)
struct PRELOGIN_VERSION {
  unsigned int version;
  unsigned short build;
};
#pragma pack()

class tdspacket_prelogin : public tdspacket {
private:
  uint32_t token_bitfield;
  PRELOGIN_VERSION token_version;
  uint8_t token_encryption;
  char *token_instance;
  uint32_t token_threadid;
  uint8_t token_mars;

protected:
  tdspacket_prelogin(uint8_t packet_type, char *buffer, size_t length, uint32_t tdsversion);
  int writepacket(dynbuffer *buffer);

  friend tdspacket *tdspacket::readpacket(int sock, uint32_t tdsversion);

public:
  using tdspacket::writepacket;

  tdspacket_prelogin(uint32_t tdsversion);
  //TDS_MESSAGE_TYPE get_packet_type() { return TDS_MESSAGE_TYPE_RESPONSE; }


  PRELOGIN_VERSION get_version() { return token_version; }
  unsigned char get_encryption() { return token_encryption; }
  char *get_instance();
  //unsigned long get_threadid();
  unsigned char get_mars() { return token_mars; }

  void set_version(PRELOGIN_VERSION value);
  void set_encryption(uint8_t value);
  void set_instance(uint8_t value);
  void set_threadid(uint32_t value);
  void set_mars(uint8_t value);
};

#endif
