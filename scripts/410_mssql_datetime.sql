CREATE FUNCTION datepart(tsql_varchar, tsql_datetime) RETURNS tsql_int AS '/home/james/tdspsql/tdspsql.so', 'tsql_datetime_datepart' LANGUAGE C IMMUTABLE STRICT;

