CREATE FUNCTION tsql_decimal_eq_decimal(tsql_decimal, tsql_decimal) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR = (PROCEDURE = tsql_decimal_eq_decimal, LEFTARG = tsql_decimal, RIGHTARG = tsql_decimal, COMMUTATOR = =, NEGATOR = !=, RESTRICT = eqsel, JOIN = eqjoinsel, HASHES, MERGES);
CREATE FUNCTION tsql_decimal_ne_decimal(tsql_decimal, tsql_decimal) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR != (PROCEDURE = tsql_decimal_ne_decimal, LEFTARG = tsql_decimal, RIGHTARG = tsql_decimal, COMMUTATOR = !=, NEGATOR = =, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE FUNCTION tsql_decimal_lt_decimal(tsql_decimal, tsql_decimal) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR < (PROCEDURE = tsql_decimal_lt_decimal, LEFTARG = tsql_decimal, RIGHTARG = tsql_decimal, COMMUTATOR = >, NEGATOR = >=, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE FUNCTION tsql_decimal_le_decimal(tsql_decimal, tsql_decimal) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR <= (PROCEDURE = tsql_decimal_le_decimal, LEFTARG = tsql_decimal, RIGHTARG = tsql_decimal, COMMUTATOR = >=, NEGATOR = >, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE FUNCTION tsql_decimal_gt_decimal(tsql_decimal, tsql_decimal) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR > (PROCEDURE = tsql_decimal_gt_decimal, LEFTARG = tsql_decimal, RIGHTARG = tsql_decimal, COMMUTATOR = <, NEGATOR = <=, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE FUNCTION tsql_decimal_ge_decimal(tsql_decimal, tsql_decimal) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR >= (PROCEDURE = tsql_decimal_ge_decimal, LEFTARG = tsql_decimal, RIGHTARG = tsql_decimal, COMMUTATOR = <=, NEGATOR = <, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);

CREATE FUNCTION tsql_decimal_cmp_decimal(tsql_decimal, tsql_decimal) RETURNS INT4 AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS tsql_decimal_ops DEFAULT FOR TYPE tsql_decimal USING btree FAMILY tsql_decimal_ops AS
 OPERATOR 1 <,
 OPERATOR 2 <=,
 OPERATOR 3 =,
 OPERATOR 4 >=,
 OPERATOR 5 >,
 FUNCTION 1 tsql_decimal_cmp_decimal(tsql_decimal, tsql_decimal);

CREATE FUNCTION tsql_decimal_ne_int(tsql_decimal, int4) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR != (
  PROCEDURE = tsql_decimal_ne_int,
  LEFTARG = tsql_decimal,
  RIGHTARG = int4,
  COMMUTATOR = !=,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel,
  HASHES,
  MERGES
);

CREATE FUNCTION tsql_decimal_eq_int(tsql_decimal, int4) RETURNS BOOL AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR = (
  PROCEDURE = tsql_decimal_eq_int,
  LEFTARG = tsql_decimal,
  RIGHTARG = int4,
  COMMUTATOR = =,
  NEGATOR = !=,
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  HASHES,
  MERGES
);

CREATE FUNCTION tsql_decimal_pl_decimal(tsql_decimal, tsql_decimal) RETURNS tsql_decimal AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
  PROCEDURE = tsql_decimal_pl_decimal,
  LEFTARG = tsql_decimal,
  RIGHTARG = tsql_decimal,
  COMMUTATOR = +
);

CREATE FUNCTION tsql_decimal_mi_decimal(tsql_decimal, tsql_decimal) RETURNS tsql_decimal AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
  PROCEDURE = tsql_decimal_mi_decimal,
  LEFTARG = tsql_decimal,
  RIGHTARG = tsql_decimal
);

CREATE FUNCTION tsql_decimal_mul_decimal(tsql_decimal, tsql_decimal) RETURNS tsql_decimal AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR * (
  PROCEDURE = tsql_decimal_mul_decimal,
  LEFTARG = tsql_decimal,
  RIGHTARG = tsql_decimal,
  COMMUTATOR = *
);

CREATE FUNCTION tsql_decimal_mul_int(tsql_decimal, int4) RETURNS tsql_decimal AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR * (
  PROCEDURE = tsql_decimal_mul_int,
  LEFTARG = tsql_decimal,
  RIGHTARG = int4,
  COMMUTATOR = *
);

-- AGGREGATES

CREATE AGGREGATE sum(tsql_decimal) (
  SFUNC = tsql_decimal_pl_decimal,
  STYPE = tsql_decimal
);

