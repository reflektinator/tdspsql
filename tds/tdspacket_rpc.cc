#include "../tdspsql.h"

tdspacket_rpc::tdspacket_rpc(uint8_t packet_type, char *buffer, size_t length, uint32_t tdsversion) : tdspacket(packet_type, buffer, length, tdsversion) {
  uint32_t header_length;
  uint16_t proc_name_length;
  uint32_t bufpos = 0;
  uint16_t flags;
  int param_count = 0;
  if (tdsversion >= 0x72000000) {
    header_length = GET_HU32(buffer, bufpos);
    bufpos += 4;
    bufpos += header_length - 4;
  }
  proc_name_length = GET_HU16(buffer, bufpos);
printf("proc_name_length = %d\n", proc_name_length);
  bufpos += 2;
  if (proc_name_length == 0xffff) {
    proc_id = GET_HU16(buffer, bufpos);
printf("proc_id = %d\n", proc_id);
    switch(proc_id) {
    case 10:
      proc = "sp_executesql";
      break;
    default:
      proc = "unknown_proc_id";
      break;
    }
    bufpos += 2;
  } else {
    proc_id = -1;
    proc = GET_STRING_UCS2(buffer, bufpos, proc_name_length, 0); // TODO this probably isn't right
    bufpos += proc_name_length;
  }
printf("proc = %s\n", proc.c_str());
  flags = GET_HU16(buffer, bufpos);
printf("flags = %02hx\n", flags);
  bufpos += 2;
  while (bufpos < message_length) {
    uint8_t param_name_length;
    string param_name;
    uint8_t param_flags;
    uint8_t param_type;

    param_name_length = GET_U8(buffer, bufpos);
printf("param_name_length = %d\n", param_name_length);
    bufpos += 1;
    param_name = GET_STRING_UCS2(buffer, bufpos, param_name_length * 2, 0);
    param_names.push_back(param_name);
    bufpos += param_name_length * 2;
printf("param_name = %s\n", param_name.c_str());
    param_flags = GET_U8(buffer, bufpos);
    bufpos += 1;
printf("param_flags = %02hhx\n", param_flags);
    param_type = GET_U8(buffer, bufpos);
printf("param_type = %hhx\n", param_type);
    bufpos++;
    switch(param_type) {
    case TSQL_XTYPE_NVARCHAR: {
      uint16_t param_length;
      printf("nvarchar\n");
      bufpos += 7;
      param_length = GET_HU16(buffer, bufpos);
printf("param_length = %hd\n", param_length);
      bufpos += 2;
      param_values.push_back(tsql_var_nvarchar(param_length, buffer + bufpos));
      bufpos += param_length;
      break; }
    default:
      printf("unknown type\n");
      break;
    }
    param_count++;
  }
}

