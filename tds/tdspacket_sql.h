#ifndef _TDSPACKET_SQL_H
#define _TDSPACKET_SQL_H

/* http://msdn.microsoft.com/en-us/library/dd358575.aspx */

#include "../tdspsql.h"

class tdspacket_sql : public tdspacket {
private:
  char *sql_query;

protected:
  //char *query;
  tdspacket_sql(uint8_t packet_type, char *buffer, size_t length, uint32_t tdsversion);
  friend tdspacket *tdspacket::readpacket(int sock, uint32_t tdsversion);

public:
  tdspacket_sql(uint32_t tdsversion);
  //TDS_MESSAGE_TYPE get_packet_type() { return TDS_MESSAGE_TYPE_SQL; }
  char *get_sql_query() { return sql_query; }
};

#endif

