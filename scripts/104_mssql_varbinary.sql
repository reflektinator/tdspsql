CREATE FUNCTION tsql_varbinary_in(cstring, oid, int4) RETURNS tsql_varbinary AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_out(tsql_varbinary) RETURNS cstring AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_typmod_in(cstring[]) RETURNS int4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_typmod_out(int4) RETURNS cstring AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_recv(internal) RETURNS tsql_varbinary AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_send(tsql_varbinary) RETURNS bytea AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE TYPE tsql_varbinary (
  INPUT = tsql_varbinary_in,
  OUTPUT = tsql_varbinary_out,
  TYPMOD_IN = tsql_varbinary_typmod_in,
  TYPMOD_OUT = tsql_varbinary_typmod_out,
  RECEIVE = tsql_varbinary_recv,
  SEND = tsql_varbinary_send,
  ALIGNMENT = 'int4',
  STORAGE = 'extended',
  INTERNALLENGTH = VARIABLE
);
