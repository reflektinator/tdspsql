PG_FUNCTION_INFO_V1(tsql_cast_@TYPE_NAME1@_@TYPE_NAME2@);

Datum
tsql_cast_@TYPE_NAME1@_@TYPE_NAME2@(PG_FUNCTION_ARGS) {
  TYPE_@TYPE_NAME1@ in = GETARG_@TYPE_NAME1@(0);
  RETURN_@TYPE_NAME2@(CAST_@TYPE_NAME2@(in));
}
