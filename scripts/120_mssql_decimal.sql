CREATE FUNCTION tsql_decimal_in(cstring, oid, int4) RETURNS tsql_decimal AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_decimal_out(tsql_decimal) RETURNS cstring AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_decimal_typmod_in(cstring[]) RETURNS int4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_decimal_typmod_out(int4) RETURNS cstring AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE tsql_decimal (
  INPUT = tsql_decimal_in,
  OUTPUT = tsql_decimal_out,
  TYPMOD_IN = tsql_decimal_typmod_in,
  TYPMOD_OUT = tsql_decimal_typmod_out,
  ALIGNMENT = 'int4',
  STORAGE = 'extended',
  INTERNALLENGTH = VARIABLE
);
