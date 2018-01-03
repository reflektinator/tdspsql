#include "../tdspsql.h"

int
tsql_executor::init() {
  vector<string> dummy_vars;
  tsql_results *results;
  sess_vars["@@fetch_status"] = tsql_var_int(0);
  sess_vars["@@microsoftversion"] = tsql_var_int(0x090013CD); // TODO: this is just a grab from MSSQL
  results = exec_int("SELECT pg_backend_pid()::tsql_int", dummy_vars, sess_vars);
  if (results->is_error()) {
    return 1;
  }
  sess_vars["@@spid"] = tsql_var(*results, 0, 0);
  delete results;
pprintf(WARNING, "done tsql_executor::init()\n");
  return 0;
}

tsql_results *
tsql_executor::exec_int(string query, vector<string>vars, map<string, tsql_var>prog_vars) {
  int res;
  Oid param_types[vars.size()];
  Datum param_vals[vars.size()];
  char param_nulls[vars.size()];
  int i;
  size_t pos = 0;
  string search = "##DATABASE##";

  while((pos = query.find(search, pos)) != string::npos) {
    query.replace(pos, search.length(), database);
    pos += database.length();
  }

  pprintf(WARNING, "exec_int(%s)", query.c_str());
  for (i = 0; i < (int)vars.size(); i++) {
    pprintf(WARNING, " param %d (%s) type %s", i, vars[i].c_str(), prog_vars[vars[i]].type.name.c_str());
    if (vars[i].substr(0, 2) == "@@") {
      param_types[i] = sess_vars[vars[i]].type.oid;
      param_vals[i] = sess_vars[vars[i]].value;
      if (sess_vars[vars[i]].isnull) {
        param_nulls[i] = 'n';
      } else {
        param_nulls[i] = ' ';
      }
    } else {
      param_types[i] = prog_vars[vars[i]].type.oid;
      param_vals[i] = prog_vars[vars[i]].value;
      if (prog_vars[vars[i]].isnull) {
        param_nulls[i] = 'n';
      } else {
        param_nulls[i] = ' ';
      }
    }
  }
  res = SPI_execute_with_args(query.c_str(), vars.size(), param_types, param_vals, param_nulls, false, 0);
  if (res >= 0) {
    return new tsql_results(SPI_tuptable, SPI_processed);
  } else {
    return NULL;
  }
    
#if 0
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    printf("201403062023\n");
    printf("error = %s\n", PQresultErrorMessage(res));
  } else {
    printf("exec_int success\n");
  }
#endif
}

int
tsql_executor::exec_ext(string query, vector<string>vars, map<string, tsql_var>prog_vars) {
  int res;
  Oid param_types[vars.size()];
  Datum param_vals[vars.size()];
  char param_nulls[vars.size()];
  int i;
  size_t pos = 0;
  string search = "##DATABASE##";

  while((pos = query.find(search, pos)) != string::npos) {
    query.replace(pos, search.length(), database);
    pos += database.length();
  }

  pprintf(WARNING, "exec_ext(%s)", query.c_str());
  for (i = 0; i < (int)vars.size(); i++) {
    pprintf(WARNING, " param %d (%s) type %s", i, vars[i].c_str(), prog_vars[vars[i]].type.name.c_str());
    if (vars[i].substr(0, 2) == "@@") {
      param_types[i] = sess_vars[vars[i]].type.oid;
      param_vals[i] = (Datum)sess_vars[vars[i]].value;
      if (sess_vars[vars[i]].isnull) {
        param_nulls[i] = 'n';
      } else {
        param_nulls[i] = ' ';
      }
    } else {
      param_types[i] = prog_vars[vars[i]].type.oid;
      param_vals[i] = (Datum)prog_vars[vars[i]].value;
      if (prog_vars[vars[i]].isnull) {
        param_nulls[i] = 'n';
      } else {
        param_nulls[i] = ' ';
      }
    }
  }
pprintf(WARNING, "executing %s (%ld vars)", query.c_str(), vars.size());
  res = SPI_execute_with_args(query.c_str(), vars.size(), param_types, param_vals, param_nulls, false, 0);
pprintf(WARNING, "res = %d\n", res);
  if (res < 0) {
#if 0
    printf("201403062024\n%s\n%s\n", query.c_str(), PQresultErrorMessage(res));
    printf("SEVERITY = %s\n", PQresultErrorField(res, PG_DIAG_SEVERITY));
    printf("SQLSTATE = %s\n", PQresultErrorField(res, PG_DIAG_SQLSTATE));
    printf("PRIMARY = %s\n", PQresultErrorField(res, PG_DIAG_MESSAGE_PRIMARY));
    printf("DETAIL = %s\n", PQresultErrorField(res, PG_DIAG_MESSAGE_DETAIL));
    printf("HINT = %s\n", PQresultErrorField(res, PG_DIAG_MESSAGE_HINT));
    printf("POSITION = %s\n", PQresultErrorField(res, PG_DIAG_STATEMENT_POSITION));
    printf("SCHEMA = %s\n", PQresultErrorField(res, PG_DIAG_SCHEMA_NAME));
    printf("TABLE = %s\n", PQresultErrorField(res, PG_DIAG_TABLE_NAME));
    printf("COLUMN = %s\n", PQresultErrorField(res, PG_DIAG_COLUMN_NAME));
    printf("TYPE = %s\n", PQresultErrorField(res, PG_DIAG_DATATYPE_NAME));
    printf("COLUMN = %s\n", PQresultErrorField(res, PG_DIAG_SEVERITY));
    if (strcasecmp(PQresultErrorField(res, PG_DIAG_SQLSTATE), "42P01") == 0) {
      error(208, 1, 16, PQresultErrorMessage(res));
    } else {
      error(1, 1, 16, PQresultErrorMessage(res));
    }
#endif
    error(1, 1, 16, "unknown error");
    return 1;
  }
  switch (res) {
  case SPI_OK_UTILITY:
  case SPI_OK_FINISH:
  case SPI_OK_INSERT:
  case SPI_OK_UPDATE:
  case SPI_OK_DELETE: {
    tdstoken_done *ttd;
    ttd = new tdstoken_done();
    ttd->set_rowcount(0);
    stpt->add_token(ttd);
    return 0; }
  case SPI_OK_SELECT: {
    tsql_results *results = new tsql_results(SPI_tuptable, SPI_processed);
    tdstoken_results *ttr;
    tdstoken_done *ttd;
    ttr = new tdstoken_results(results, &types);
    stpt->add_token(ttr);
    ttd = new tdstoken_done();
    ttd->set_rowcount(SPI_processed);
    stpt->add_token(ttd);
    //PQclear(res);
    return 0; }
  default: {
    error(1, 1, 16, "Internal error");
    printf("201504261604 - Unknown res %d\n", res);
    return 1; }
  }
}

int
tsql_executor::exec_cmd(string query, vector<string>vars, map<string, tsql_var>prog_vars) {
  int res;
  Oid param_types[vars.size()];
  Datum param_vals[vars.size()];
  char param_nulls[vars.size()];
  int i;
  size_t pos = 0;
  string search = "##DATABASE##";

  while((pos = query.find(search, pos)) != string::npos) {
    query.replace(pos, search.length(), database);
    pos += database.length();
  }

  pprintf(WARNING, "exec_cmd(%s)", query.c_str());
  for (i = 0; i < (int)vars.size(); i++) {
    pprintf(WARNING, " param %d (%s) type %s", i, vars[i].c_str(), prog_vars[vars[i]].type.name.c_str());
    if (vars[i].substr(0, 2) == "@@") {
      param_types[i] = sess_vars[vars[i]].type.oid;
      param_vals[i] = sess_vars[vars[i]].value;
      if (sess_vars[vars[i]].isnull) {
        param_nulls[i] = 'n';
      } else {
        param_nulls[i] = ' ';
      }
    } else {
      param_types[i] = prog_vars[vars[i]].type.oid;
      param_vals[i] = prog_vars[vars[i]].value;
      if (prog_vars[vars[i]].isnull) {
        param_nulls[i] = 'n';
      } else {
        param_nulls[i] = ' ';
      }
    }
  }
  res = SPI_execute_with_args(query.c_str(), vars.size(), param_types, param_vals, param_nulls, false, 0);
  if (res >= 0) {
    return 0;
  } else {
    return 1;
  }
}

int
tsql_executor::exec_nul() {
  return 0;
}

int
tsql_executor::exec_bool(string expr, vector<string>vars, map<string, tsql_var>prog_vars, int *result) {
  int res;
  Oid param_types[vars.size()];
  Datum param_vals[vars.size()];
  char param_nulls[vars.size()];
  int i;
  size_t pos = 0;
  string search = "##DATABASE##";
  string query = "SELECT ";

  while((pos = expr.find(search, pos)) != string::npos) {
    query.replace(pos, search.length(), database);
    pos += database.length();
  }
  query += expr;

  pprintf(WARNING, "exec_bool(%s)", query.c_str());
  for (i = 0; i < (int)vars.size(); i++) {
    pprintf(WARNING, " param %d (%s) type %s", i, vars[i].c_str(), prog_vars[vars[i]].type.name.c_str());
    if (vars[i].substr(0, 2) == "@@") {
      param_types[i] = sess_vars[vars[i]].type.oid;
      param_vals[i] = sess_vars[vars[i]].value;
      if (sess_vars[vars[i]].isnull) {
        param_nulls[i] = 'n';
      } else {
        param_nulls[i] = ' ';
      }
    } else {
      param_types[i] = prog_vars[vars[i]].type.oid;
      param_vals[i] = prog_vars[vars[i]].value;
      if (prog_vars[vars[i]].isnull) {
        param_nulls[i] = 'n';
      } else {
        param_nulls[i] = ' ';
      }
    }
  }
  res = SPI_execute_with_args(query.c_str(), vars.size(), param_types, param_vals, param_nulls, false, 0);
pprintf(WARNING, "res = %d, SPI_processed = %d", res, SPI_processed);
  if (res >= 0) {
    if (SPI_processed > 1) {
      return 1;
    } else if (SPI_processed == 1) {
      bool isnull;
      pprintf(WARNING, "SPI_getbinval = %c", (char)SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1, &isnull));
      pprintf(WARNING, "SPI_getbinval = %d", (int)SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1, &isnull));
      pprintf(WARNING, "isnull = %d", isnull);
      *result = !!SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1, &isnull);
      if (isnull) {
        *result = 0;
      }
pprintf(WARNING, "*result = %d", *result);
      return 0;
    }
  } else {
    return 1;
  }
#if 0
  if (PQntuples(res) != 1 || PQnfields(res) != 1) {
    printf("201403062027\n");
    PQclear(res);
    return 1;
  }
  if (PQgetvalue(res, 0, 0)[0] == 't') {
printf("is true\n");
    *result = 1;
  } else {
printf("is false\n");
    *result = 0;
  }
  PQclear(res);
  
#endif
  return 0;
}

void
tsql_executor::error(uint32_t error_number, uint8_t error_state, uint8_t error_severity, const char *message) {
  tdstoken_error *tte = new tdstoken_error();
  tdstoken_done *ttd;
  tte->set_error_number(error_number);
  tte->set_error_state(error_state);
  tte->set_error_severity(error_severity);
  tte->set_error_line_number(1);
  tte->set_error_message(strdup(message));
  stpt->add_token(tte);
  ttd = new tdstoken_done();
  //ttd->set_rowcount(0);
  ttd->set_status_error(1);
  //ttd->set_curcmd(0x00fd);
  stpt->add_token(ttd);
}

void
tsql_executor::info(uint32_t info_number, uint8_t info_state, uint8_t info_severity, const char *message) {
  tdstoken_info *tti = new tdstoken_info();
  tdstoken_done *ttd;
  tti->set_info_number(info_number);
  tti->set_info_state(info_state);
  tti->set_info_severity(info_severity);
  tti->set_info_line_number(1);
  tti->set_info_message(strdup(message));
  stpt->add_token(tti);
  ttd = new tdstoken_done();
  //ttd->set_rowcount(0);
  //ttd->set_status_error(0);
  stpt->add_token(ttd);
}

void
tsql_executor::set_database(string database) {
  tdstoken_env_database *tted = new tdstoken_env_database();
  tted->set_old_database(this->database);
  tted->set_new_database(database);
  stpt->add_token(tted);
  tdstoken_env_collation *ttec = new tdstoken_env_collation();
  stpt->add_token(ttec);
  this->database = database;
}
