CREATE FUNCTION tsql_cast_decimal_tinyint(tsql_decimal) RETURNS tsql_tinyint AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_decimal as tsql_tinyint) WITH FUNCTION tsql_cast_decimal_tinyint(tsql_decimal);
CREATE FUNCTION tsql_cast_decimal_smallint(tsql_decimal) RETURNS tsql_smallint AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_decimal as tsql_smallint) WITH FUNCTION tsql_cast_decimal_smallint(tsql_decimal);
CREATE FUNCTION tsql_cast_decimal_int(tsql_decimal) RETURNS tsql_int AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_decimal as tsql_int) WITH FUNCTION tsql_cast_decimal_int(tsql_decimal);

CREATE FUNCTION tsql_cast_smallint_decimal(tsql_smallint) RETURNS tsql_decimal AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_smallint AS tsql_decimal) WITH FUNCTION tsql_cast_smallint_decimal(tsql_smallint) AS IMPLICIT;
CREATE FUNCTION tsql_cast_int_decimal(tsql_int) RETURNS tsql_decimal AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_int AS tsql_decimal) WITH FUNCTION tsql_cast_int_decimal(tsql_int) AS IMPLICIT;

CREATE FUNCTION tsql_cast_decimal_float(tsql_decimal) RETURNS tsql_float AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_decimal as tsql_float) WITH FUNCTION tsql_cast_decimal_float(tsql_decimal) AS ASSIGNMENT;
CREATE FUNCTION tsql_cast_float_decimal(tsql_float) RETURNS tsql_decimal AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_float AS tsql_decimal) WITH FUNCTION tsql_cast_float_decimal(tsql_float) AS IMPLICIT;
