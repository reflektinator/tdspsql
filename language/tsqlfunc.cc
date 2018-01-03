#include "../tdspsql.h"

extern "C" {
PG_FUNCTION_INFO_V1(tsql_func_language);
Datum tsql_func_language(PG_FUNCTION_ARGS);
}

Datum
tsql_func_language(PG_FUNCTION_ARGS) {
  // http://www.postgresql.org/docs/9.3/static/error-message-reporting.html
  // fcinfo->flinfo->fn_oid is oid of actual function in pg_proc
  if (CALLED_AS_TRIGGER(fcinfo)) {
    ereport(ERROR, (errcode(ERRCODE_SYSTEM_ERROR), errmsg("trigger not supported")));
    // we don't support this right now
    PG_RETURN_VOID();
  } else {
    //tsql_results results;
    char *query;
    char *function;
    SPI_connect();
    init_types();
    asprintf(&query, "SELECT pg_proc.prosrc, pg_proc.pronargs FROM pg_proc JOIN pg_language ON pg_proc.prolang = pg_language.oid WHERE pg_proc.oid = %d", fcinfo->flinfo->fn_oid);
    SPI_execute(query, 0, 1);
    if (!SPI_processed) {
      elog(ERROR, "function not found");
    }
    function = SPI_getvalue(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1);
pprintf(WARNING, "function =\n%s", function);
    /*tsql_program *program =*/ tsql_parse(function);
    //program->exec(exec, prog_vars);
    /*elog(ERROR, "Not supported");*/
    //ereport(ERROR, (errcode(ERRCODE_SYSTEM_ERROR), errmsg("here we are")));
    SPI_finish();
    PG_RETURN_INT32(69);
  }
}

