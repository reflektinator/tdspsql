DROP FUNCTION IF EXISTS "master.dbo".sp_addlogin ("@loginname" tsql_sysname, "@passwd" tsql_sysname);
CREATE FUNCTION "master.dbo"."sp_addlogin" ("@loginname" tsql_sysname, "@passwd" tsql_sysname) RETURNS tsql_int AS
$TSQL$
DECLARE @sql varchar(8000)
SET @sql = 'CREATE LOGIN ' + @loginname + ' WITH PASSWORD ''' + @passwd + ''''
EXEC (@sql)
$TSQL$ LANGUAGE tsqlproc;

DROP FUNCTION IF EXISTS "master.dbo".sp_adduser ("@username" tsql_sysname);
CREATE FUNCTION "master.dbo".sp_adduser ("@username" tsql_sysname) RETURNS tsql_int AS
$TSQL$
DECLARE @sql varchar(8000)
SET @sql = 'CREATE USER ' + @username 
EXEC (@sql)
$TSQL$ LANGUAGE tsqlproc;

DROP FUNCTION IF EXISTS "master.dbo".sp_grantdbaccess ("@username" tsql_sysname);
CREATE FUNCTION "master.dbo".sp_grantdbaccess ("@username" tsql_sysname) RETURNS tsql_int AS
$TSQL$
DECLARE @sql varchar(8000)
SET @sql = 'CREATE USER ' + @username 
EXEC (@sql)
$TSQL$ LANGUAGE tsqlproc;

DROP FUNCTION IF EXISTS "master.dbo".sp_addrole ("@rolename" tsql_sysname);
CREATE FUNCTION "master.dbo".sp_addrole ("@rolename" tsql_sysname) RETURNS tsql_int AS
$TSQL$
-- TODO this does nothing right now
$TSQL$ LANGUAGE tsqlproc;

DROP FUNCTION IF EXISTS "master.dbo".sp_addsrvrolemember ("@username" tsql_sysname, "@rolename" tsql_sysname);
CREATE FUNCTION "master.dbo".sp_addsrvrolemember ("@username" tsql_sysname, "@rolename" tsql_sysname) RETURNS tsql_int AS
$TSQL$
-- TODO this does nothing right now
$TSQL$ LANGUAGE tsqlproc;

DROP FUNCTION IF EXISTS "master.dbo".is_srvrolemember ("@rolename" tsql_sysname);
CREATE FUNCTION "master.dbo".is_srvrolemember ("@rolename" tsql_sysname) RETURNS tsql_int AS
$TSQL$
SELECT 1::tsql_int
$TSQL$ LANGUAGE tsqlfunc;

DROP FUNCTION IF EXISTS "master.dbo".has_dbaccess("@dbname" tsql_sysname);
CREATE FUNCTION "master.dbo".has_dbaccess("@dbname" tsql_sysname) RETURNS tsql_int AS
$TSQL$
SELECT 1::tsql_int
$TSQL$ LANGUAGE sql;

DROP FUNCTION IF EXISTS "master.dbo".sp_dbcmptlevel ("@dbname" tsql_sysname, "@level" tsql_int);
CREATE FUNCTION "master.dbo".sp_dbcmptlevel ("@dbname" tsql_sysname, "@level" tsql_int) RETURNS tsql_int AS
$TSQL$
-- TODO this does nothing right now
$TSQL$ LANGUAGE tsqlproc;

