CREATE FUNCTION tsql_nvarchar_in(cstring, oid, int4) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_nvarchar_out(tsql_nvarchar) RETURNS cstring AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_nvarchar_recv(internal) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_nvarchar_send(tsql_nvarchar) RETURNS bytea AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE TYPE tsql_nvarchar (
  INPUT = tsql_nvarchar_in,
  OUTPUT = tsql_nvarchar_out,
  TYPMOD_IN = tsql_varchar_typmod_in,
  TYPMOD_OUT = tsql_varchar_typmod_out,
  RECEIVE = tsql_nvarchar_recv,
  SEND = tsql_nvarchar_send,
  ALIGNMENT = 'int4',
  STORAGE = 'extended',
  INTERNALLENGTH = VARIABLE,
  COLLATABLE = true
);
