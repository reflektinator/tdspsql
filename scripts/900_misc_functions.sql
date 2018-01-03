DROP FUNCTION IF EXISTS str(tsql_float, tsql_int, tsql_int);
CREATE FUNCTION str(tsql_float, tsql_int, tsql_int) RETURNS tsql_varchar AS '/home/james/tdspsql/tdspsql.so', 'tsql_float_str' LANGUAGE C IMMUTABLE STRICT;

DROP FUNCTION IF EXISTS round(tsql_float, tsql_int);
CREATE FUNCTION round(tsql_float, tsql_int) RETURNS tsql_float AS '/home/james/tdspsql/tdspsql.so', 'tsql_float_round' LANGUAGE C IMMUTABLE STRICT;

DROP FUNCTION IF EXISTS db_name();
CREATE FUNCTION db_name()
 RETURNS tsql_nvarchar
 LANGUAGE plpgsql
AS $function$
BEGIN
 RETURN 'master'::tsql_nvarchar;
END
$function$;

DROP FUNCTION IF EXISTS tsql_object_to_pg_object(tsql_nvarchar, tsql_nvarchar, tsql_nvarchar);
CREATE FUNCTION tsql_object_to_pg_object("@object_name" tsql_nvarchar, "@db_name" tsql_nvarchar, "@schema_name" tsql_nvarchar)
 RETURNS tsql_nvarchar
AS '/home/james/tdspsql/tdspsql.so' LANGUAGE C;

DROP FUNCTION IF EXISTS object_id("@object_name" tsql_nvarchar);
CREATE FUNCTION object_id("@object_name" tsql_nvarchar)
 RETURNS tsql_int
AS $function$
SELECT tsql_object_to_pg_object($1,
  split_part(current_schema, '.', 1)::tsql_nvarchar,
  split_part(current_schema, '.', 2)::tsql_nvarchar)::varchar::regclass::oid::int::tsql_int;
$function$ LANGUAGE sql;

DROP FUNCTION IF EXISTS collationproperty("@name" tsql_sysname, "@property" tsql_sysname);
CREATE FUNCTION collationproperty("@name" tsql_sysname, "@property" tsql_sysname)
 RETURNS tsql_int
AS $function$
SELECT 1033::tsql_int;
$function$ LANGUAGE sql;

DROP FUNCTION IF EXISTS databasepropertyex(tsql_nvarchar, tsql_nvarchar);
CREATE FUNCTION databasepropertyex(tsql_nvarchar, tsql_nvarchar)
 RETURNS tsql_nvarchar
 LANGUAGE plpgsql
AS $function$
BEGIN
 CASE UPPER($2::text)
  WHEN 'SQLCHARSET' THEN RETURN 1;
  ELSE RETURN 'UNKNOWN';
 END CASE;
END
$function$;

DROP FUNCTION IF EXISTS serverproperty(tsql_nvarchar);
CREATE FUNCTION serverproperty(tsql_nvarchar)
 RETURNS tsql_nvarchar(128)
 LANGUAGE sql
AS $function$
SELECT CASE UPPER($1)
 WHEN 'PRODUCTVERSION' THEN '10.50.4000.0'::tsql_nvarchar(128)
 WHEN 'PRODUCTLEVEL' THEN 'SP2'::tsql_nvarchar(128)
 WHEN 'ENGINEEDITION' THEN '4'::tsql_nvarchar(128)
 WHEN 'EDITION' THEN 'Enterprise Edition (64-bit)'::tsql_nvarchar(128)
 ELSE 'UNKNOWN'::tsql_nvarchar(128)
END
$function$;

DROP FUNCTION IF EXISTS sp_bcp_dbcmptlevel(tsql_nvarchar);
CREATE FUNCTION sp_bcp_dbcmptlevel("@dbname" tsql_nvarchar)
 RETURNS int
AS $tsql$
SELECT CAST(80 AS tinyint)
$tsql$ LANGUAGE tsqlproc;

DROP FUNCTION IF EXISTS sp_tablecollations(tsql_nvarchar);
CREATE FUNCTION sp_tablecollations("@object" tsql_nvarchar(4000))
 RETURNS int
AS $tsql$
SELECT colid, syscolumns.name, tdscollation tds_collation, collation collation_name
FROM syscolumns
WHERE syscolumns.id = object_id(@object)
$tsql$ LANGUAGE tsqlproc;

DROP FUNCTION IF EXISTS "master.sys".fn_helpcollations();
CREATE FUNCTION "master.sys".fn_helpcollations()
 RETURNS TABLE (name tsql_sysname, description tsql_nvarchar(2000))
AS $tsql$
 SELECT N'Latin1_General_CI_AS'::tsql_sysname, N'Latin1-General, case-insensitive, accent-sensitive, kanatype-insensitive, width-insensitive'::tsql_nvarchar(2000)
$tsql$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS "master.dbo".xp_instance_regread("@root" tsql_sysname, "@key" tsql_sysname, "@name" tsql_sysname);
CREATE FUNCTION "master.dbo".xp_instance_regread(OUT tsql_int, "@root" tsql_sysname, "@key" tsql_sysname, "@name" tsql_sysname, OUT "@value" tsql_sysname)
AS $function$
BEGIN
 IF @name = 'SqlArg0' THEN
  SET @value = '-dC:\TSQL\master.mdf'
 ELSE IF @name = 'SqlArg1' THEN
  SET @value = '-eC:\TSQL\ERRORLOG'
 ELSE IF @name = 'SqlArg2' THEN
  SET @value = '-lC:\Program Files\Microsoft SQL Server\MSSQL10_50.MSSQLSERVER\MSSQL\DATA\mastlog.ldf'
 ELSE
  SET @value = NULL
END
$function$ LANGUAGE tsqlproc; 

