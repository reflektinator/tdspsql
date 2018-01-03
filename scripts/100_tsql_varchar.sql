CREATE FUNCTION tsql_varchar_in(cstring, oid, int4) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_out(tsql_varchar) RETURNS cstring AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_typmod_in(cstring[]) RETURNS int4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_typmod_out(int4) RETURNS cstring AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_recv(internal) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_send(tsql_varchar) RETURNS bytea AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE TYPE tsql_varchar (
  INPUT = tsql_varchar_in,
  OUTPUT = tsql_varchar_out,
  TYPMOD_IN = tsql_varchar_typmod_in,
  TYPMOD_OUT = tsql_varchar_typmod_out,
  RECEIVE = tsql_varchar_recv,
  SEND = tsql_varchar_send,
  ALIGNMENT = 'int4',
  STORAGE = 'extended',
  INTERNALLENGTH = VARIABLE,
  COLLATABLE = true
);
