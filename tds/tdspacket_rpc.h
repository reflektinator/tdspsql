#ifndef _TDSPACKET_RPC_H
#define _TDSPACKET_RPC_H

#include "../tdspsql.h"

class tdspacket_rpc : public tdspacket {
private:
  string proc;
  int proc_id;
  vector<string> param_names;
  vector<tsql_var> param_values;

protected:
  //char *query;
  tdspacket_rpc(uint8_t packet_type, char *buffer, size_t length, uint32_t tdsversion);
  friend tdspacket *tdspacket::readpacket(int sock, uint32_t tdsversion);

public:
  tdspacket_rpc(uint32_t tdsversion);
  //TDS_MESSAGE_TYPE get_packet_type() { return TDS_MESSAGE_TYPE_RPC; }
  string &get_proc() { return proc; }
  int get_proc_id() { return proc_id; }
  vector<string> &get_param_names() { return param_names; }
  vector<tsql_var> &get_param_values() { return param_values; }
};

#endif

