#ifndef _BUFFER_H
#define _BUFFER_H

#include "../tdspsql.h"

#define DYNBUFFER_LENGTH_TYPE_NONE 0
#define DYNBUFFER_LENGTH_TYPE_NU16 1
#define DYNBUFFER_LENGTH_TYPE_HU16 2

#define DYNBUFFER_POINTER_TYPE_NU16  1

struct tailbuffer {
  tailbuffer *next;
  char *buffer;
  size_t buffer_size;
  int pointer_type;
  size_t pointer_pos;
  size_t pointer_offset;
};

class dynbuffer {
private:
  dynbuffer *parent;
  char *buffer;
  tailbuffer *tailbuffer_head;
  tailbuffer *tailbuffer_tail;
  size_t buffer_size;
  size_t buffer_pos;
  size_t length_pos;
  int length_offset;
  int length_type;
  int finalised;

  void init();
  int make_room(size_t size);

protected:

public:
  dynbuffer();
  dynbuffer(dynbuffer *buffer);
  dynbuffer(size_t initial_size);
  int length() { return buffer_pos; }
  void finalise();
  int write(int sock);
  int write(int sock, int offset, int length);

  char *getbuffer() { return buffer; }

  int add_length_nu16(int offset = 0);
  int add_length_hu16(int offset = 0);

  int add_bytes(const char *bytes, size_t size);
  int add_bytes(const char *bytes, size_t size, int pointer_type, int pointer_offset);
  int add_padding(size_t value);

  /* add_<e><s><len> - e = endian (<n>et or <h>ost>), s = sign/unsigned, len = 8/16/32 */
  int add_u8(uint8_t value);
  int add_nu16(uint16_t value);
  int add_hu16(uint16_t value);
  int add_nu32(uint32_t value);
  int add_hu32(uint32_t value);

  /* add_<type><order><len>_<encoding> - type = cc for character counted with length prefix, len = bytes for length, encoding = ucs-2 */
  int add_cc1_ucs2(const char *value); /* value is utf-8 */
  int add_cch2_ucs2(const char *value); /* value is utf-8 */
  int add_cch2_utf8(const char *value); /* value is utf-8 */
  int add_cch4_utf8(const char *value); /* value is utf-8 */

  /* add the length and offset to some data at the end of the buffer */
  int add_offset_bytes_nu16(char *bytes, size_t size);
  int add_offset_bytes_u8(uint8_t value);
};

#endif

