-- CASTS

CREATE FUNCTION tsql_cast_varchar_datetime(tsql_varchar) RETURNS tsql_datetime AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_varchar AS tsql_datetime) WITH FUNCTION tsql_cast_varchar_datetime(tsql_varchar) AS IMPLICIT;

-- OPERATORS

CREATE FUNCTION tsql_datetime_eq_datetime(tsql_datetime, tsql_datetime) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR = ( PROCEDURE = tsql_datetime_eq_datetime,
  LEFTARG = tsql_datetime, RIGHTARG = tsql_datetime,
  COMMUTATOR = =, NEGATOR = !=,
  RESTRICT = eqsel, JOIN = eqjoinsel,
  HASHES, MERGES 
);
CREATE FUNCTION tsql_datetime_ne_datetime(tsql_datetime, tsql_datetime) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR != ( PROCEDURE = tsql_datetime_ne_datetime,
  LEFTARG = tsql_datetime, RIGHTARG = tsql_datetime,
  COMMUTATOR = !=, NEGATOR = =,
  RESTRICT = neqsel, JOIN = neqjoinsel
);
CREATE FUNCTION tsql_datetime_lt_datetime(tsql_datetime, tsql_datetime) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR < ( PROCEDURE = tsql_datetime_lt_datetime,
  LEFTARG = tsql_datetime, RIGHTARG = tsql_datetime,
  COMMUTATOR = >, NEGATOR = >=,
  RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE FUNCTION tsql_datetime_le_datetime(tsql_datetime, tsql_datetime) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR <= (PROCEDURE = tsql_datetime_le_datetime,
  LEFTARG = tsql_datetime, RIGHTARG = tsql_datetime,
  COMMUTATOR = >=, NEGATOR = >,
  RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE FUNCTION tsql_datetime_gt_datetime(tsql_datetime, tsql_datetime) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR > (PROCEDURE = tsql_datetime_gt_datetime,
  LEFTARG = tsql_datetime, RIGHTARG = tsql_datetime,
  COMMUTATOR = <, NEGATOR = <=,
  RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);
CREATE FUNCTION tsql_datetime_ge_datetime(tsql_datetime, tsql_datetime) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR >= (PROCEDURE = tsql_datetime_ge_datetime,
  LEFTARG = tsql_datetime, RIGHTARG = tsql_datetime,
  COMMUTATOR = <=, NEGATOR = <,
  RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);
CREATE FUNCTION tsql_datetime_cmp(tsql_datetime, tsql_datetime) RETURNS INT4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS tsql_datetime_ops DEFAULT FOR TYPE tsql_datetime USING btree FAMILY tsql_datetime_ops AS
 OPERATOR 1 <,
 OPERATOR 2 <=,
 OPERATOR 3 =,
 OPERATOR 4 >=,
 OPERATOR 5 >,
 FUNCTION 1 tsql_datetime_cmp(tsql_datetime, tsql_datetime);

-- AGGREGATES

CREATE FUNCTION tsql_datetime_max(tsql_datetime, tsql_datetime) RETURNS tsql_datetime AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE max(tsql_datetime) (
  SFUNC = tsql_datetime_max,
  STYPE = tsql_datetime
);

-- OTHER

CREATE FUNCTION tsql_coalesce(tsql_datetime, tsql_datetime) RETURNS tsql_datetime AS '/home/james/tdspsql/tdspsql.so', 'tsql_datetime_coalesce' LANGUAGE C IMMUTABLE;

CREATE FUNCTION convert(tsql_varchar, tsql_datetime, int4) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_datetime_convert' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION getdate() RETURNS tsql_datetime AS '/home/james/tdspsql/tdspsql.so', 'tsql_datetime_getdate' LANGUAGE C IMMUTABLE STRICT;

