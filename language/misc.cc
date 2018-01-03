#include "../tdspsql.h"

tsql_types types;

void init_types() {
  if (types.initialised) {
    return;
  }
  char *query;
  int result;
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
  types.initialised = 1;
}

