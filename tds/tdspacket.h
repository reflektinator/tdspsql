#ifndef _TDSPACKET_H
#define _TDSPACKET_H

#include "../tdspsql.h"
#if 0
#include "dynbuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

union packet_status_t {
  struct {
    uint8_t eom_flag:1;
    uint8_t ignore_flag:1;
    uint8_t unused2:1;
    uint8_t reset_flag:1; /* 7.1 */
    uint8_t reset_skip_tran_flag:1; /* 7.2 */
    uint8_t unused5:1;
    uint8_t unused6:1;
    uint8_t unused7:1;
  };
  uint8_t status_byte;
};

enum TDS_MESSAGE_TYPE {
  TDS_MESSAGE_TYPE_SQL      = 0x01,
  TDS_MESSAGE_TYPE_LOGIN4   = 0x02,
  TDS_MESSAGE_TYPE_RPC      = 0x03,
  TDS_MESSAGE_TYPE_RESPONSE = 0x04,
  TDS_MESSAGE_TYPE_LOGIN7   = 0x10,
  TDS_MESSAGE_TYPE_PRELOGIN = 0x12,
  TDS_MESSAGE_TYPE_INVALID  = -1,
};

class tdspacket {
private:
  int readonly;
  size_t max_buffer_length;

protected:
  tdspacket(uint8_t packet_type, char *buffer, size_t length, uint32_t tdsversion);
  size_t message_length;
  uint32_t tdsversion;
  uint8_t packet_type;

public:
  tdspacket(uint32_t tdsversion);
  virtual ~tdspacket() {};

  static tdspacket* readpacket(int sock, uint32_t tdsversion);
  int writepacket(int sock);
  virtual int writepacket(dynbuffer *buffer) { return 0; }

  virtual TDS_MESSAGE_TYPE get_packet_type() { return (TDS_MESSAGE_TYPE)packet_type; }

  //char *get_buffer() { if (!readonly) return buffer; else return NULL; }

  uint16_t get_message_length() { return message_length; }
};

#endif

