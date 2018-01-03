-- CASTS

--CREATE FUNCTION tsql_cast_varchar_ptext(tsql_varchar) RETURNS text AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (tsql_varchar AS text) WITH FUNCTION tsql_cast_varchar_text(tsql-varchar) AS ASSIGNMENT;
--CREATE FUNCTION tsql_cast_ptext_varchar(text) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (text AS tsql_varchar) WITH FUNCTION tsql_cast_ptext_varchar(text) AS IMPLICIT;
CREATE FUNCTION tsql_cast_uniqueidentifier_varchar(uuid) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (uuid AS tsql_varchar) WITH FUNCTION tsql_cast_uniqueidentifier_varchar(uuid) AS IMPLICIT;
--CREATE FUNCTION tsql_cast_int_varchar(int4) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (int4 AS tsql_varchar) WITH FUNCTION tsql_cast_int_varchar(int4) AS IMPLICIT;
--CREATE FUNCTION tsql_cast_varchar_int(tsql_varchar) RETURNS int4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (tsql_varchar AS int4) WITH FUNCTION tsql_cast_varchar_int(tsql_varchar) AS ASSIGNMENT;
--CREATE FUNCTION tsql_cast_float_varchar(float8) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (float8 AS tsql_varchar) WITH FUNCTION tsql_cast_float_varchar(float8) AS IMPLICIT;

CREATE FUNCTION tsql_cast_varchar_varchar(tsql_varchar, int) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_varchar AS tsql_varchar) WITH FUNCTION tsql_cast_varchar_varchar(tsql_varchar, int) AS IMPLICIT;

-- OPERATORS

CREATE FUNCTION tsql_varchar_eq_varchar(tsql_varchar, tsql_varchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_ne_varchar(tsql_varchar, tsql_varchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_lt_varchar(tsql_varchar, tsql_varchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_le_varchar(tsql_varchar, tsql_varchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_gt_varchar(tsql_varchar, tsql_varchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_ge_varchar(tsql_varchar, tsql_varchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varchar_cmp(tsql_varchar, tsql_varchar) RETURNS INT4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR = (PROCEDURE = tsql_varchar_eq_varchar, LEFTARG = tsql_varchar, RIGHTARG = tsql_varchar, COMMUTATOR = =, NEGATOR = !=, RESTRICT = eqsel, JOIN = eqjoinsel, MERGES);
CREATE OPERATOR != (PROCEDURE = tsql_varchar_ne_varchar, LEFTARG = tsql_varchar, RIGHTARG = tsql_varchar, COMMUTATOR = !=, NEGATOR = =, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OPERATOR < (PROCEDURE = tsql_varchar_lt_varchar, LEFTARG = tsql_varchar, RIGHTARG = tsql_varchar, COMMUTATOR = >, NEGATOR = >=, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OPERATOR <= (PROCEDURE = tsql_varchar_le_varchar, LEFTARG = tsql_varchar, RIGHTARG = tsql_varchar, COMMUTATOR = >=, NEGATOR = >, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OPERATOR > (PROCEDURE = tsql_varchar_gt_varchar, LEFTARG = tsql_varchar, RIGHTARG = tsql_varchar, COMMUTATOR = <, NEGATOR = <=, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OPERATOR >= (PROCEDURE = tsql_varchar_ge_varchar, LEFTARG = tsql_varchar, RIGHTARG = tsql_varchar, COMMUTATOR = <=, NEGATOR = <, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OPERATOR CLASS tsql_varchar_ops DEFAULT FOR TYPE tsql_varchar USING btree FAMILY tsql_char_ops AS
 OPERATOR 1 <,
 OPERATOR 2 <=,
 OPERATOR 3 =,
 OPERATOR 4 >=,
 OPERATOR 5 >,
 FUNCTION 1 tsql_varchar_cmp(tsql_varchar, tsql_varchar);

-- TODO - this is just = for now
CREATE OPERATOR ~~ (PROCEDURE = tsql_varchar_eq_varchar, LEFTARG = tsql_varchar, RIGHTARG = tsql_varchar, COMMUTATOR = =, NEGATOR = !=, RESTRICT = likesel, JOIN = likejoinsel);

CREATE FUNCTION tsql_varchar_pl_varchar(tsql_varchar, tsql_varchar) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (PROCEDURE = tsql_varchar_pl_varchar, LEFTARG = tsql_varchar, RIGHTARG = tsql_varchar, COMMUTATOR = +);

CREATE FUNCTION upper(tsql_varchar) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_varchar_upper' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION lower(tsql_varchar) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_varchar_lower' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION ltrim(tsql_varchar) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_varchar_rtrim' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION rtrim(tsql_varchar) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_varchar_rtrim' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION left(tsql_varchar, tsql_int) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_varchar_left' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION len(tsql_varchar) RETURNS tsql_int AS '/home/james/tdspsql/tdspsql.so', 'tsql_varchar_len' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION replace(tsql_varchar, tsql_varchar, tsql_varchar) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_varchar_replace' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_coalesce(tsql_varchar, tsql_varchar) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_varchar_coalesce' LANGUAGE C IMMUTABLE;

-- AGGREGATES

CREATE FUNCTION tsql_varchar_max(tsql_varchar, tsql_varchar) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE max(tsql_varchar) (
  SFUNC = tsql_varchar_max,
  STYPE = tsql_varchar
);

