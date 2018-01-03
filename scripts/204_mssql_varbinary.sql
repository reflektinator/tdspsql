-- CASTS

CREATE FUNCTION tsql_cast_varbinary_varbinary(tsql_varbinary, int) RETURNS tsql_varbinary AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (tsql_varbinary AS tsql_varbinary) WITH FUNCTION tsql_cast_varbinary_varbinary(tsql_varbinary, int) AS IMPLICIT;

-- OPERATORS

CREATE FUNCTION tsql_varbinary_eq_varbinary(tsql_varbinary, tsql_varbinary) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_ne_varbinary(tsql_varbinary, tsql_varbinary) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_lt_varbinary(tsql_varbinary, tsql_varbinary) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_le_varbinary(tsql_varbinary, tsql_varbinary) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_gt_varbinary(tsql_varbinary, tsql_varbinary) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_ge_varbinary(tsql_varbinary, tsql_varbinary) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tsql_varbinary_cmp(tsql_varbinary, tsql_varbinary) RETURNS INT4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR = (PROCEDURE = tsql_varbinary_eq_varbinary, LEFTARG = tsql_varbinary, RIGHTARG = tsql_varbinary,
  COMMUTATOR = =, NEGATOR = !=, RESTRICT = eqsel, JOIN = eqjoinsel, MERGES);
CREATE OPERATOR != (PROCEDURE = tsql_varbinary_ne_varbinary, LEFTARG = tsql_varbinary, RIGHTARG = tsql_varbinary,
  COMMUTATOR = !=, NEGATOR = =, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OPERATOR < (PROCEDURE = tsql_varbinary_lt_varbinary, LEFTARG = tsql_varbinary, RIGHTARG = tsql_varbinary,
  COMMUTATOR = >, NEGATOR = >=, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OPERATOR <= (PROCEDURE = tsql_varbinary_le_varbinary, LEFTARG = tsql_varbinary, RIGHTARG = tsql_varbinary,
  COMMUTATOR = >=, NEGATOR = >, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OPERATOR > (PROCEDURE = tsql_varbinary_gt_varbinary, LEFTARG = tsql_varbinary, RIGHTARG = tsql_varbinary,
  COMMUTATOR = <, NEGATOR = <=, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OPERATOR >= (PROCEDURE = tsql_varbinary_ge_varbinary, LEFTARG = tsql_varbinary, RIGHTARG = tsql_varbinary,
  COMMUTATOR = <=, NEGATOR = <, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OPERATOR CLASS tsql_varbinary_ops DEFAULT FOR TYPE tsql_varbinary USING btree FAMILY tsql_binary_ops AS
 OPERATOR 1 <,
 OPERATOR 2 <=,
 OPERATOR 3 =,
 OPERATOR 4 >=,
 OPERATOR 5 >,
 FUNCTION 1 tsql_varbinary_cmp(tsql_varbinary, tsql_varbinary);

-- TODO - this is just = for now
CREATE OPERATOR ~~ (PROCEDURE = tsql_varbinary_eq_varbinary, LEFTARG = tsql_varbinary, RIGHTARG = tsql_varbinary,
 COMMUTATOR = =, NEGATOR = !=, RESTRICT = likesel, JOIN = likejoinsel);

CREATE FUNCTION tsql_varbinary_pl_varbinary(tsql_varbinary, tsql_varbinary) RETURNS tsql_varbinary AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (PROCEDURE = tsql_varbinary_pl_varbinary, LEFTARG = tsql_varbinary, RIGHTARG = tsql_varbinary, COMMUTATOR = +);

CREATE FUNCTION tsql_coalesce(tsql_varbinary, tsql_varbinary) RETURNS tsql_varbinary AS '/home/james/tdspsql/tdspsql.so', 'tsql_varbinary_coalesce' LANGUAGE C IMMUTABLE;

-- AGGREGATES

CREATE FUNCTION tsql_varbinary_max(tsql_varbinary, tsql_varbinary) RETURNS tsql_varbinary AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE max(tsql_varbinary) (
  SFUNC = tsql_varbinary_max,
  STYPE = tsql_varbinary
);

