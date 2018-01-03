DROP SCHEMA IF EXISTS "master.dbo" CASCADE;
DROP SCHEMA IF EXISTS "master.sys" CASCADE;
DROP TYPE IF EXISTS tsql_varchar CASCADE;
DROP TYPE IF EXISTS tsql_nvarchar CASCADE;
DROP TYPE IF EXISTS tsql_varbinary CASCADE;
DROP TYPE IF EXISTS tsql_datetime CASCADE;
DROP TYPE IF EXISTS tsql_bit CASCADE;
DROP TYPE IF EXISTS tsql_tinyint CASCADE;
DROP TYPE IF EXISTS tsql_smallint CASCADE;
DROP TYPE IF EXISTS tsql_int CASCADE;
DROP TYPE IF EXISTS tsql_bigint CASCADE;
DROP TYPE IF EXISTS tsql_float CASCADE;
DROP TYPE IF EXISTS tsql_real CASCADE;
DROP TYPE IF EXISTS tsql_decimal CASCADE;
DROP FUNCTION IF EXISTS tsql_varchar_typmod_in(cstring[]) CASCADE;
DROP FUNCTION IF EXISTS tsql_varchar_typmod_out(int4) CASCADE;
DROP FUNCTION IF EXISTS tsql_varbinary_typmod_in(cstring[]) CASCADE;
DROP FUNCTION IF EXISTS tsql_varbinary_typmod_out(int4) CASCADE;
DROP FUNCTION IF EXISTS tsql_decimal_typmod_in(cstring[]) CASCADE;
DROP FUNCTION IF EXISTS tsql_decimal_typmod_out(int4) CASCADE;
DROP DOMAIN IF EXISTS tsql_timestamp CASCADE;
DROP DOMAIN IF EXISTS tsql_image CASCADE;
DROP DOMAIN IF EXISTS tsql_money CASCADE;
DROP DOMAIN IF EXISTS tsql_sysname CASCADE;
DROP OPERATOR FAMILY IF EXISTS tsql_char_ops USING BTREE CASCADE;
DROP OPERATOR FAMILY IF EXISTS tsql_binary_ops USING BTREE CASCADE;
DROP OPERATOR FAMILY IF EXISTS tsql_int_ops USING BTREE CASCADE;
DROP OPERATOR FAMILY IF EXISTS tsql_decimal_ops USING BTREE CASCADE;

DROP FUNCTION IF EXISTS tsql_func_language() CASCADE;
DROP FUNCTION IF EXISTS tsql_proc_language() CASCADE;

CREATE SCHEMA "master.dbo";
GRANT ALL ON SCHEMA "master.dbo" to sa;
ALTER DEFAULT PRIVILEGES IN SCHEMA "master.dbo" GRANT ALL ON TABLES TO sa;

CREATE SCHEMA "master.sys";
GRANT ALL ON SCHEMA "master.sys" to sa;
ALTER DEFAULT PRIVILEGES IN SCHEMA "master.sys" GRANT ALL ON TABLES TO sa;

CREATE TYPE tsql_varchar;
CREATE TYPE tsql_nvarchar;
CREATE TYPE tsql_varbinary;
CREATE TYPE tsql_datetime;
CREATE TYPE tsql_bit;
CREATE TYPE tsql_tinyint;
CREATE TYPE tsql_smallint;
CREATE TYPE tsql_int;
CREATE TYPE tsql_bigint;
CREATE TYPE tsql_real;
CREATE TYPE tsql_float;
CREATE TYPE tsql_decimal;

CREATE OPERATOR FAMILY tsql_char_ops USING btree;
CREATE OPERATOR FAMILY tsql_binary_ops USING btree;
CREATE OPERATOR FAMILY tsql_int_ops USING btree;
CREATE OPERATOR FAMILY tsql_decimal_ops USING btree;

CREATE FUNCTION tsql_func_language() RETURNS language_handler AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C;
CREATE FUNCTION tsql_proc_language() RETURNS language_handler AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C;
CREATE TRUSTED LANGUAGE tsqlfunc HANDLER tsql_func_language;
CREATE TRUSTED LANGUAGE tsqlproc HANDLER tsql_proc_language;

GRANT ALL ON LANGUAGE tsqlfunc TO sa;
GRANT ALL ON LANGUAGE tsqlproc TO sa;
