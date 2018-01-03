-- CASTS

--CREATE FUNCTION tsql_cast_nvarchar_ptext(tsql_nvarchar) RETURNS text AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (tsql_nvarchar AS text) WITH FUNCTION tsql_cast_nvarchar_text(tsql-nvarchar) AS ASSIGNMENT;
--CREATE FUNCTION tsql_cast_ptext_nvarchar(text) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (text AS tsql_nvarchar) WITH FUNCTION tsql_cast_ptext_nvarchar(text) AS IMPLICIT;
CREATE FUNCTION tsql_cast_uniqueidentifier_nvarchar(uuid) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (uuid AS tsql_nvarchar) WITH FUNCTION tsql_cast_uniqueidentifier_nvarchar(uuid) AS IMPLICIT;
--CREATE FUNCTION tsql_cast_int_nvarchar(int4) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (int4 AS tsql_nvarchar) WITH FUNCTION tsql_cast_int_nvarchar(int4) AS IMPLICIT;
--CREATE FUNCTION tsql_cast_nvarchar_int(tsql_nvarchar) RETURNS int4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (tsql_nvarchar AS int4) WITH FUNCTION tsql_cast_nvarchar_int(tsql_nvarchar) AS ASSIGNMENT;
--CREATE FUNCTION tsql_cast_float_nvarchar(float8) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
--CREATE CAST (float8 AS tsql_nvarchar) WITH FUNCTION tsql_cast_float_nvarchar(float8) AS IMPLICIT;

CREATE FUNCTION tsql_cast_nvarchar_nvarchar(tsql_nvarchar, int) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_nvarchar AS tsql_nvarchar) WITH FUNCTION tsql_cast_nvarchar_nvarchar(tsql_nvarchar, int) AS IMPLICIT;

CREATE FUNCTION tsql_cast_varchar_nvarchar(tsql_varchar, int) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_varchar AS tsql_nvarchar) WITH FUNCTION tsql_cast_varchar_nvarchar(tsql_varchar, int) AS IMPLICIT;

-- OPERATORS

CREATE FUNCTION tsql_nvarchar_eq_nvarchar(tsql_nvarchar, tsql_nvarchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_nvarchar_ne_nvarchar(tsql_nvarchar, tsql_nvarchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_nvarchar_lt_nvarchar(tsql_nvarchar, tsql_nvarchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_nvarchar_le_nvarchar(tsql_nvarchar, tsql_nvarchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_nvarchar_gt_nvarchar(tsql_nvarchar, tsql_nvarchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_nvarchar_ge_nvarchar(tsql_nvarchar, tsql_nvarchar) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_nvarchar_cmp(tsql_nvarchar, tsql_nvarchar) RETURNS INT4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR = (PROCEDURE = tsql_nvarchar_eq_nvarchar, LEFTARG = tsql_nvarchar, RIGHTARG = tsql_nvarchar, COMMUTATOR = =, NEGATOR = !=, RESTRICT = eqsel, JOIN = eqjoinsel, MERGES);
CREATE OPERATOR != (PROCEDURE = tsql_nvarchar_ne_nvarchar, LEFTARG = tsql_nvarchar, RIGHTARG = tsql_nvarchar, COMMUTATOR = !=, NEGATOR = =, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OPERATOR < (PROCEDURE = tsql_nvarchar_lt_nvarchar, LEFTARG = tsql_nvarchar, RIGHTARG = tsql_nvarchar, COMMUTATOR = >, NEGATOR = >=, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OPERATOR <= (PROCEDURE = tsql_nvarchar_le_nvarchar, LEFTARG = tsql_nvarchar, RIGHTARG = tsql_nvarchar, COMMUTATOR = >=, NEGATOR = >, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OPERATOR > (PROCEDURE = tsql_nvarchar_gt_nvarchar, LEFTARG = tsql_nvarchar, RIGHTARG = tsql_nvarchar, COMMUTATOR = <, NEGATOR = <=, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OPERATOR >= (PROCEDURE = tsql_nvarchar_ge_nvarchar, LEFTARG = tsql_nvarchar, RIGHTARG = tsql_nvarchar, COMMUTATOR = <=, NEGATOR = <, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OPERATOR CLASS tsql_nvarchar_ops DEFAULT FOR TYPE tsql_nvarchar USING btree FAMILY tsql_char_ops AS
 OPERATOR 1 <,
 OPERATOR 2 <=,
 OPERATOR 3 =,
 OPERATOR 4 >=,
 OPERATOR 5 >,
 FUNCTION 1 tsql_nvarchar_cmp(tsql_nvarchar, tsql_nvarchar);

-- TODO - this is just = for now
CREATE OPERATOR ~~ (PROCEDURE = tsql_nvarchar_eq_nvarchar, LEFTARG = tsql_nvarchar, RIGHTARG = tsql_nvarchar, COMMUTATOR = =, NEGATOR = !=, RESTRICT = likesel, JOIN = likejoinsel);

CREATE FUNCTION tsql_nvarchar_pl_nvarchar(tsql_nvarchar, tsql_nvarchar) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (PROCEDURE = tsql_nvarchar_pl_nvarchar, LEFTARG = tsql_nvarchar, RIGHTARG = tsql_nvarchar, COMMUTATOR = +);

CREATE FUNCTION upper(tsql_nvarchar) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_nvarchar_upper' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION lower(tsql_nvarchar) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_nvarchar_lower' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION ltrim(tsql_nvarchar) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_nvarchar_rtrim' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION rtrim(tsql_nvarchar) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_nvarchar_rtrim' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION left(tsql_nvarchar, tsql_int) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_nvarchar_left' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION len(tsql_nvarchar) RETURNS tsql_int AS '/home/james/tdspsql/tdspsql.so', 'tsql_nvarchar_len' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION replace(tsql_nvarchar, tsql_nvarchar, tsql_nvarchar) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_nvarchar_replace' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_coalesce(tsql_nvarchar, tsql_nvarchar) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_nvarchar_coalesce' LANGUAGE C IMMUTABLE;

-- AGGREGATES

CREATE FUNCTION tsql_nvarchar_max(tsql_nvarchar, tsql_nvarchar) RETURNS tsql_nvarchar AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE max(tsql_nvarchar) (
  SFUNC = tsql_nvarchar_max,
  STYPE = tsql_nvarchar
);

