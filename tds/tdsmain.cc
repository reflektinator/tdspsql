#include "../tdspsql.h"

tdsmain::tdsmain(int sock, sockaddr_in sockaddr) {
  this->sock = sock;
  this->sockaddr = sockaddr;

  state = STATE_PRELOGIN;
}

void
tdsmain::run() {
  tdspacket *rtp;
  char *instance_name = NULL;
  tsql_executor exec;
  uint32_t tdsversion = 0;

pprintf(WARNING, "started tdsmain");
  while (state != STATE_FAIL && (rtp = tdspacket::readpacket(sock, tdsversion)) != NULL) {

    pprintf(WARNING, " packet_type       = %02x", rtp->get_packet_type());
    if (state == STATE_PRELOGIN && rtp->get_packet_type() == TDS_MESSAGE_TYPE_PRELOGIN) {
      tdspacket_prelogin *rtpp = (tdspacket_prelogin *)rtp;
      pprintf(WARNING, "STATE_PRELOGIN");
      pprintf(WARNING, "MESSAGE_TYPE_PRELOGIN");
      PRELOGIN_VERSION prelogin_version = rtpp->get_version();
      pprintf(WARNING, "version = %d.%d", prelogin_version.version, prelogin_version.build);
      pprintf(WARNING, "encryption = %d", rtpp->get_encryption());
      if (rtpp->get_instance() == NULL || strlen(rtpp->get_instance()) == 0 || strcasecmp(rtpp->get_instance(), "MSSQLServer") == 0) {
        pprintf(WARNING, "instance = <default>");
        instance_name = strdup("mssqlserver");
      } else {
        pprintf(WARNING, "instance = %s", rtpp->get_instance());
        instance_name = strdup(rtpp->get_instance());
      }

      pprintf(WARNING, "mars = %d", rtpp->get_mars());

      tdspacket_prelogin *stpp = new tdspacket_prelogin(tdsversion);
      //stpp->set(FIELD_MESSAGE_TYPE, (void *)TDS_MESSAGE_TYPE_RESPONSE);
      //stpp->set_packet_status_eom(1);
      PRELOGIN_VERSION version = {0x00000008, 0x0000};
      stpp->set_version(version);
      stpp->set_encryption(TDS_PRELOGIN_ENCRYPT_NOT_SUPP);
      stpp->set_instance(0); /* 0 = instance was valid */
      stpp->set_threadid(0); /* value doesn't matter - is sent empty */
      stpp->set_mars(0);
      stpp->writepacket(sock);
      state = STATE_LOGIN;
    } else if ((state == STATE_PRELOGIN && rtp->get_packet_type() == TDS_MESSAGE_TYPE_LOGIN4) ||
               (state == STATE_LOGIN && rtp->get_packet_type() == TDS_MESSAGE_TYPE_LOGIN7)) {
      tdspacket_login *rtpl = (tdspacket_login *)rtp;
      pprintf(WARNING, "STATE_LOGIN");
      if (rtp->get_packet_type() == TDS_MESSAGE_TYPE_LOGIN4) {
        pprintf(WARNING, "TDS_MESSAGE_TYPE_LOGIN4");
        instance_name = strdup("mssqlserver");
      } else {
        pprintf(WARNING, "TDS_MESSAGE_TYPE_LOGIN7");
      }
#if 0
      if (rtpl->get_packet_type() != TDS_MESSAGE_TYPE_LOGIN7) {
        pprintf(WARNING, "message_type %d != %d", rtpl->get_packet_type(), TDS_MESSAGE_TYPE_LOGIN7);
        close(sock);
        return;
      }
#endif
      pprintf(WARNING, " length = %d", rtpl->get_login_length());
      pprintf(WARNING, " tdsversion = %08x", rtpl->get_login_tdsversion());
      pprintf(WARNING, " packetsize = %d", rtpl->get_login_packetsize());
      pprintf(WARNING, " flags1 = %02x", rtpl->get_login_flags1());
      pprintf(WARNING, " flags2 = %02x", rtpl->get_login_flags2());
      pprintf(WARNING, " typeflags = %02x", rtpl->get_login_typeflags());
      pprintf(WARNING, " extraflags = %02x", rtpl->get_login_extraflags());
      pprintf(WARNING, " hostname = %s", rtpl->get_login_hostname());
      pprintf(WARNING, " username = %s", rtpl->get_login_username());
      //printf(" password = %s", rtpl->get_login_password());
      pprintf(WARNING, " database = %s", rtpl->get_login_database());

      char *tmpusername = strdup(rtpl->get_login_username());
      for (int i = 0; i < (int)strlen(tmpusername); i++) {
        tmpusername[i] = tolower(tmpusername[i]);
      }
      // TODO validate password;
      exec.username = tmpusername;
      free(tmpusername);
      if (rtpl->get_login_database() == NULL || strlen(rtpl->get_login_database()) == 0) {
        exec.database = strdup("master");
        pprintf(WARNING, "using database master (should really be default database for the user!)");
      } else {
        exec.database = strdup(rtpl->get_login_database());
        pprintf(WARNING, "using database %s", exec.database.c_str());
      }
      pprintf(WARNING, "AAA %s", instance_name);
      BackgroundWorkerInitializeConnection(instance_name, (char *)exec.username.c_str());
      pprintf(WARNING, "BBB");
      int result;
      //SetCurrentStatementStartTimestamp();
      StartTransactionCommand();
      result = SPI_connect();
      PushActiveSnapshot(GetTransactionSnapshot());
      pprintf(WARNING, "CCC");
      pprintf(WARNING, "SPI_connect = %d", result);

#if 0
      char *conn_string;
      asprintf(&conn_string, "host=%s user=%s password=%s dbname=%s", "localhost", exec.username.c_str(), rtpl->get_login_password(), dbname);
      pprintf(WARNING, "conn_string = \"%s\"", conn_string);
      exec.conn = PQconnectdb(conn_string);
      if (PQstatus(exec.conn) != CONNECTION_OK) {
        pprintf(WARNING, "Connect failed: %s", PQerrorMessage(exec.conn));
        state = STATE_FAIL;
        break;
      }
#endif
      char *query;
      asprintf(&query, "SET SEARCH_PATH TO \"%s.%s\", \"master.dbo\", public", exec.database.c_str(), "dbo");
      pprintf(WARNING, "query = %s", query);
      result = SPI_execute(query, 0, 0);
      pprintf(WARNING, "SPI_execute(%s) = %d", query, result);
      free(query);
#if 0
      query = strdup("SELECT typname, oid, typlen, typbyval, typstorage FROM pg_type WHERE typbasetype = 0 AND typname LIKE 'tsql_%'");
      result = SPI_execute(query, 0, 0);
      pprintf(WARNING, "SPI_execute(%s) = %d", query, result);
      TupleDesc tupdesc = SPI_tuptable->tupdesc;
      SPITupleTable *tuptable = SPI_tuptable;
      for (int i = 0; i < (int)SPI_processed; i++) {
        HeapTuple tuple = tuptable->vals[i];
        char *typname = SPI_getvalue(tuple, tupdesc, 1);
        string tsql_name = typname + 5;
        Oid oid = atoi(SPI_getvalue(tuple, tupdesc, 2));
        uint16_t size = (uint16_t)atoi(SPI_getvalue(tuple, tupdesc, 3));
        int byval = (SPI_getvalue(tuple, tupdesc, 3)[0] != 'f')?1:0;
        char storage = SPI_getvalue(tuple, tupdesc, 3)[0];
        if (strcasecmp(typname, "tsql_char") == 0) {
          types.add(oid, TSQL_XTYPE_CHAR, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_varchar") == 0) {
          types.add(oid, TSQL_XTYPE_VARCHAR, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_nchar") == 0) {
          types.add(oid, TSQL_XTYPE_NCHAR, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_nvarchar") == 0) {
          types.add(oid, TSQL_XTYPE_NVARCHAR, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_varbinary") == 0) {
          types.add(oid, TSQL_XTYPE_VARBINARY, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_bit") == 0) {
          types.add(oid, TSQL_XTYPE_BIT, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_tinyint") == 0) {
          types.add(oid, TSQL_XTYPE_TINYINT, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_smallint") == 0) {
          types.add(oid, TSQL_XTYPE_SMALLINT, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_int") == 0) {
          types.add(oid, TSQL_XTYPE_INT, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_bigint") == 0) {
          types.add(oid, TSQL_XTYPE_BIGINT, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_float") == 0) {
          types.add(oid, TSQL_XTYPE_FLOAT, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_real") == 0) {
          types.add(oid, TSQL_XTYPE_REAL, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_decimal") == 0) {
          types.add(oid, TSQL_XTYPE_DECIMAL, tsql_name, size, byval, storage);
        } else if (strcasecmp(typname, "tsql_datetime") == 0) {
          types.add(oid, TSQL_XTYPE_DATETIME, tsql_name, size, byval, storage);
        } else {
          pprintf(WARNING, "Unknown type %s = %s", SPI_getvalue(tuple, tupdesc, 1), SPI_getvalue(tuple, tupdesc, 2));
        }
      }
      /* now add domains using the xtype of the base type */
      query = strdup("SELECT typname, oid, typbasetype FROM pg_type WHERE typbasetype <> 0 AND typname LIKE 'tsql_%'");
      result = SPI_execute(query, 0, 0);
      pprintf(WARNING, "SPI_execute(%s) = %d", query, result);
      tupdesc = SPI_tuptable->tupdesc;
      tuptable = SPI_tuptable;
      for (int i = 0; i < (int)SPI_processed; i++) {
        HeapTuple tuple = tuptable->vals[i];
        char *typname = SPI_getvalue(tuple, tupdesc, 1);
        string tsql_name = typname + 6;
        tsql_type basetype = types.by_oid[atoi(SPI_getvalue(tuple, tupdesc, 3))];
        types.add(atoi(SPI_getvalue(tuple, tupdesc, 2)), basetype.xtype, tsql_name, basetype.size, basetype.byval, basetype.storage);
      }
#endif
      init_types();
      tdstoken_loginack *ttla = new tdstoken_loginack();
      ttla->set_interface(1);
      pprintf(WARNING, " received tdsversion = %08x", rtpl->get_login_tdsversion());
      if (rtpl->get_login_tdsversion() > TDSVERSION_SERVER) {
        pprintf(WARNING, " downgrading tdsversion to %08x", TDSVERSION_SERVER);
        tdsversion = TDSVERSION_SERVER;
      } else {
        pprintf(WARNING, " using client tdsversion");
        tdsversion = rtpl->get_login_tdsversion();
      }
      exec.init();
      ttla->set_tdsversion(tdsversion);
      //ttla->set_serverproduct("tdspsql");
      ttla->set_serverproduct("Microsoft SQL Server");
      tdspacket_tokenised *stpt = new tdspacket_tokenised(tdsversion);
      stpt->add_token(ttla);
      exec.stpt = stpt;
      exec.set_database(exec.database);
      tdstoken_done *ttd = new tdstoken_done();
      stpt->add_token(ttd);
      SPI_finish();
      PopActiveSnapshot();
      CommitTransactionCommand();
      stpt->writepacket(sock);
      state = STATE_CONNECTED;
    } else if (state == STATE_CONNECTED) {
      switch(rtp->get_packet_type()) { 
      case TDS_MESSAGE_TYPE_SQL: {
        tdspacket_sql *rtps = (tdspacket_sql *)rtp;
        tdspacket_tokenised *stpt = new tdspacket_tokenised(tdsversion);
        tsql_program *program = tsql_parse(rtps->get_sql_query());
        if (program->error_message) {
          pprintf(WARNING, "ERROR: %s", rtps->get_sql_query());
          tdstoken_error *tte = new tdstoken_error();
          tte->set_error_number(102);
          tte->set_error_state(1);
          tte->set_error_severity(15);
          tte->set_error_line_number(program->first_line);
          tte->set_error_message(program->error_message);
          stpt->add_token(tte);
          tdstoken_done *ttd = new tdstoken_done();;
          ttd->set_status_error(1);
          stpt->add_token(ttd);
          stpt->writepacket(sock);
        } else {
          StartTransactionCommand();
          SPI_connect();
          PushActiveSnapshot(GetTransactionSnapshot());
          exec.stpt = stpt;
          MemoryContext oldcontext = CurrentMemoryContext;
          ResourceOwner oldowner = CurrentResourceOwner;
          BeginInternalSubTransaction(NULL);
          MemoryContextSwitchTo(oldcontext);
          PG_TRY();
          {
            if (program->exec(&exec)) {
              pprintf(WARNING, "ERROR: %s", rtps->get_sql_query());
            }
            pprintf(WARNING, "SUCCESS: %s", rtps->get_sql_query());
            stpt->writepacket(sock);
            ReleaseCurrentSubTransaction();
            MemoryContextSwitchTo(oldcontext);
            CurrentResourceOwner = oldowner;
            SPI_restore_connection();
          }
          PG_CATCH();
          {
            MemoryContextSwitchTo(oldcontext);
            ErrorData *edata = CopyErrorData();
            FlushErrorState();
            RollbackAndReleaseCurrentSubTransaction();
            MemoryContextSwitchTo(oldcontext);
            CurrentResourceOwner = oldowner;
            SPI_restore_connection();
            pprintf(WARNING, "ERROR: %s", rtps->get_sql_query());
            pprintf(WARNING, "message: %s", edata->message);
            pprintf(WARNING, "detail: %s", edata->detail);
            pprintf(WARNING, "sqlerrcode: %08x", edata->sqlerrcode);
            pprintf(WARNING, "elevel: %08x", edata->elevel);
            tdstoken_error *tte = new tdstoken_error();
            tte->set_error_number(102);
            tte->set_error_state(1);
            tte->set_error_severity(15);
            tte->set_error_line_number(1);
            tte->set_error_message(edata->message);
            stpt->add_token(tte);
            tdstoken_done *ttd = new tdstoken_done();;
            ttd->set_status_error(1);
            stpt->add_token(ttd);
            stpt->writepacket(sock);
          }
          PG_END_TRY();
          SPI_finish();
          PopActiveSnapshot();
          CommitTransactionCommand();
        }
        delete program;
        break; }
      case TDS_MESSAGE_TYPE_RPC: {
        tdspacket_rpc *rtpr = (tdspacket_rpc *)rtp;
pprintf(WARNING, "proc = %s", rtpr->get_proc().c_str());
pprintf(WARNING, "proc_id = %d", rtpr->get_proc_id());
pprintf(WARNING, "param_count = %ld", rtpr->get_param_names().size());
for (int i = 0; i < (int)rtpr->get_param_names().size(); i++) {
  pprintf(WARNING, "param[%d] name = %s", i, rtpr->get_param_names()[i].c_str());
}
        switch(rtpr->get_proc_id()) {
        case 10: {
          map<string, tsql_var> prog_vars;
          string query;
          tsql_var param;
          param = rtpr->get_param_values()[1];
          query = "DECLARE ";
          query += GET_STRING_UCS2(((char *)param.value) + VARHDRSZ, 0, VARSIZE(param.value) - VARHDRSZ, 0);
          query += "\n";
          param = rtpr->get_param_values()[0];
          query += GET_STRING_UCS2(((char *)param.value) + VARHDRSZ, 0, VARSIZE(param.value) - VARHDRSZ, 0);
pprintf(WARNING, "PROGRAM: %s", query.c_str());
          /* sp_executesql */
          tdspacket_tokenised *stpt = new tdspacket_tokenised(tdsversion);
          tsql_program *program = tsql_parse(query.c_str());

          for (int i = 2; i < (int)rtpr->get_param_names().size(); i++) {
//pprintf(WARNING, "%s type = %d", rtpr->get_param_names()[i].c_str(), rtpr->get_param_values()[i].type);
            prog_vars[rtpr->get_param_names()[i]] = rtpr->get_param_values()[i];
//pprintf(WARNING, "%s type = %d", rtpr->get_param_names()[i].c_str(), prog_vars[rtpr->get_param_names()[i]].type);
            //query += prefix + rtpr->get_param_names()[i];
            //prefix = ", ";
          }

          exec.stpt = stpt;
          StartTransactionCommand();
          SPI_connect();
          PushActiveSnapshot(GetTransactionSnapshot());
          if (program->exec(&exec, prog_vars)) {
            pprintf(WARNING, "ERROR");
          }
          SPI_finish();
          PopActiveSnapshot();
          CommitTransactionCommand();
          stpt->writepacket(sock);
          delete program;
          break; }
        case -1: {
          pprintf(WARNING, "unknown proc id %d", rtpr->get_proc_id());
          break; }
        default: {
          pprintf(WARNING, "unknown proc id %d", rtpr->get_proc_id());
          break; }
        }
          
//printf("query = %s", 

#if 0
sp_executesql "SELECT blah from blah WHERE blah = @parm1", "@parm1 varchar(10)", @parm1="blah"
becomes
DECLARE @parm1 varchar(1)
EXEC sp_executesql @parm1
then fill in variables
then run

need rpc style statement class?
then subclass for sp_executesql
param1 is name of proc
param2 is variable declaration - might need to parse that
param3-N is parameters
load variable values into vars list
give them dummy names, maybe
then exec
#endif
        break; }
      default:
        pprintf(WARNING, "unknown packet type");
        break;
      }
    } else {
      state = STATE_FAIL;
    }
    delete rtp;
  }
  pprintf(WARNING, "thread closing");
  close(sock);
  return;
}

