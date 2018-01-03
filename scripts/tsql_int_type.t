CREATE FUNCTION tsql_@TYPE_NAME@_in(cstring) RETURNS tsql_@TYPE_NAME@ AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_@TYPE_NAME@_out(tsql_@TYPE_NAME@) RETURNS cstring AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_@TYPE_NAME@_recv(internal) RETURNS tsql_@TYPE_NAME@ AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_@TYPE_NAME@_send(tsql_@TYPE_NAME@) RETURNS bytea AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE TYPE tsql_@TYPE_NAME@ (
  INPUT = tsql_@TYPE_NAME@_in,
  OUTPUT = tsql_@TYPE_NAME@_out,
  RECEIVE = tsql_@TYPE_NAME@_recv,
  SEND = tsql_@TYPE_NAME@_send,
  STORAGE = 'plain',
  INTERNALLENGTH = @LENGTH@,
  @BYVALUE@
  ALIGNMENT = '@ALIGNMENT@'
);

