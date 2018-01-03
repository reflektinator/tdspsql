CREATE FUNCTION tsql_datetime_in(cstring) RETURNS tsql_datetime AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_datetime_out(tsql_datetime) RETURNS cstring AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_datetime_recv(internal) RETURNS tsql_datetime AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_datetime_send(tsql_datetime) RETURNS bytea AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE TYPE tsql_datetime (
  INPUT = tsql_datetime_in,
  OUTPUT = tsql_datetime_out,
  RECEIVE = tsql_datetime_recv,
  SEND = tsql_datetime_send,
  ALIGNMENT = 'int4',
  STORAGE = 'plain',
  INTERNALLENGTH = 8
);
