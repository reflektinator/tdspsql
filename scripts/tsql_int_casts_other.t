CREATE FUNCTION tsql_cast_int4_@TYPE_NAME@(int4) RETURNS tsql_@TYPE_NAME@ AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (int4 AS tsql_@TYPE_NAME@) WITH FUNCTION tsql_cast_int4_@TYPE_NAME@(int4) AS IMPLICIT;
CREATE FUNCTION tsql_cast_int8_@TYPE_NAME@(int8) RETURNS tsql_@TYPE_NAME@ AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (int8 AS tsql_@TYPE_NAME@) WITH FUNCTION tsql_cast_int8_@TYPE_NAME@(int8) AS IMPLICIT;
--CREATE FUNCTION tsql_cast_tinyint_int(tsql_@TYPE_NAME@) RETURNS int4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (tsql_@TYPE_NAME@ AS int4) WITH FUNCTION tsql_cast_tinyint_int(tsql_@TYPE_NAME@) AS IMPLICIT;
CREATE FUNCTION tsql_cast_varchar_@TYPE_NAME@(tsql_varchar) RETURNS tsql_@TYPE_NAME@ AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_varchar AS tsql_@TYPE_NAME@) WITH FUNCTION tsql_cast_varchar_@TYPE_NAME@(tsql_varchar);
CREATE FUNCTION tsql_cast_nvarchar_@TYPE_NAME@(tsql_nvarchar) RETURNS tsql_@TYPE_NAME@ AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_nvarchar AS tsql_@TYPE_NAME@) WITH FUNCTION tsql_cast_nvarchar_@TYPE_NAME@(tsql_nvarchar);

CREATE FUNCTION tsql_cast_@TYPE_NAME@_varchar(tsql_@TYPE_NAME@) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_@TYPE_NAME@ AS tsql_varchar) WITH FUNCTION tsql_cast_@TYPE_NAME@_varchar(tsql_@TYPE_NAME@) AS IMPLICIT;
CREATE FUNCTION tsql_cast_@TYPE_NAME@_float8(tsql_@TYPE_NAME@) RETURNS float8 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_@TYPE_NAME@ AS float8) WITH FUNCTION tsql_cast_@TYPE_NAME@_float8(tsql_@TYPE_NAME@) AS IMPLICIT;

