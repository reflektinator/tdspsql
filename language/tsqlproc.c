#include <postgres.h>
#include "executor/spi.h"
#include "commands/trigger.h"
#include <catalog/pg_type.h>
#include <utils/array.h>
#include <utils/builtins.h>
#include <fmgr.h>
#include <string.h>
#include <math.h>

PG_FUNCTION_INFO_V1(tsql_proc_language);

/* proc language will never do anything - procs are executed by the tds processor */
// TODO - it might be possible to allow this if we can accept that no select statements will return results
Datum
tsql_proc_language(PG_FUNCTION_ARGS) {
  ereport(ERROR, (errcode(ERRCODE_SYSTEM_ERROR), errmsg("tsql procedures cannot be called this way")));
}

