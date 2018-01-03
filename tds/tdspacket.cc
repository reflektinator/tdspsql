#include "../tdspsql.h"

tdspacket::tdspacket(uint8_t packet_type, char *buffer, size_t length, uint32_t tdsversion) {
  readonly = 1;
  message_length = length;
  this->tdsversion = tdsversion;
  this->packet_type = packet_type;
}

tdspacket::tdspacket(uint32_t tdsversion) {
  readonly = 0;
  message_length = 0;
  this->tdsversion = tdsversion;
}

tdspacket *
tdspacket::readpacket(int sock, uint32_t tdsversion) {
  char header[sizeof(TDS_MESSAGE_HEADER)];
  char *buffer;
  char *bufptr;
  int res;
  int remaining;
  uint16_t packet_length;
  size_t total_length;
  uint16_t packet_type = (uint16_t)-1;
  packet_status_t packet_status;

  buffer = new char[1024 * 1024];
  bufptr = buffer;
  total_length = 0;
  do {
    res = ::recv(sock, header, sizeof(TDS_MESSAGE_HEADER), MSG_WAITALL);
    if (res != sizeof(TDS_MESSAGE_HEADER)) {
      return NULL;
    }
    if (packet_type == (uint16_t)-1) {
      packet_type = GET_U8(header, 0);
    } else {
      if (packet_type != (uint16_t)GET_U8(header, 0)) {
        printf("packet type mismatch %d != %d\n", packet_type, GET_U8(header, 0));
      }
    }
    packet_status.status_byte = GET_U8(header, 1);
    packet_length = GET_NU16(header, 2);
    remaining = packet_length - sizeof(TDS_MESSAGE_HEADER);
    total_length += remaining;
    while (remaining) {
      res = ::recv(sock, bufptr, remaining, MSG_WAITALL);
      if (res == -1) {
        printf("read error\n");
        return NULL;
      }
      bufptr += res;
      remaining -= res;
    }
  } while (!packet_status.eom_flag);
//printf("total_length = %d\n", (int)total_length);
  switch (packet_type) {
  case TDS_MESSAGE_TYPE_PRELOGIN:
    //printf("new PRELOGIN packet\n");
    return new tdspacket_prelogin(packet_type, buffer, total_length, tdsversion);
  case TDS_MESSAGE_TYPE_LOGIN4:
    //printf("new LOGIN4 packet\n");
    return new tdspacket_login(packet_type, buffer, total_length, tdsversion);
  case TDS_MESSAGE_TYPE_LOGIN7:
    //printf("new LOGIN7 packet\n");
    return new tdspacket_login(packet_type, buffer, total_length, tdsversion);
  case TDS_MESSAGE_TYPE_SQL:
    //printf("new SQL packet\n");
    return new tdspacket_sql(packet_type, buffer, total_length, tdsversion);
  case TDS_MESSAGE_TYPE_RPC:
    //printf("new RPC packet\n");
    return new tdspacket_rpc(packet_type, buffer, total_length, tdsversion);
  default:
    printf("no such type %d\n", packet_type);
    return NULL;
  }
}

int
tdspacket::writepacket(int sock) {
  dynbuffer *buffer = new dynbuffer();
  packet_status_t packet_status;
  packet_status.status_byte = 0;

  if (!writepacket(buffer)) {
    return 0;
  }
  buffer->finalise();

  int remaining = buffer->length();
  int write_pos = 0;
  int max_packet_size = 4096;
  while (remaining > 0) {
    dynbuffer *header_buffer = new dynbuffer();
    int packet_size;
    if (remaining > max_packet_size - 8) {
      packet_size = max_packet_size - 8;
    } else {
      packet_size = remaining;
      packet_status.eom_flag = 1;
    }
//printf("packet_size = %d\n", packet_size);
    header_buffer->add_u8(get_packet_type());
    header_buffer->add_u8(packet_status.status_byte);
    header_buffer->add_nu16(packet_size + 8);
    header_buffer->add_nu16(0); /* spid */
    header_buffer->add_u8(1); /* packetid */
    header_buffer->add_u8(0); /* window */
    header_buffer->write(sock);
//printf("writing offset %d length %d\n", write_pos, packet_size);
    buffer->write(sock, write_pos, packet_size);
    write_pos += packet_size;
    remaining -= packet_size;
  }
  return 1;
}

