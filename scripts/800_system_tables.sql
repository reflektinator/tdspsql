DROP TABLE IF EXISTS "master.dbo".syscharsets;
CREATE TABLE "master.dbo".syscharsets (type tsql_smallint, id tsql_tinyint, csid tsql_tinyint, status tsql_smallint, name tsql_sysname,
  description tsql_nvarchar(255), binarydefinition tsql_varbinary(255), definition tsql_varbinary(255));
INSERT INTO "master.dbo".syscharsets VALUES (1001::tsql_smallint, 1::tsql_tinyint, 0::tsql_tinyint , 0::tsql_smallint, 'iso_1'::tsql_nvarchar,
  'ISO 8859-1 (Latin-1) - Western Eurpoean 8 bit character set.'::tsql_nvarchar, NULL::tsql_varbinary, NULL::tsql_varbinary);

DROP TABLE IF EXISTS "master.dbo".syslanguages;
CREATE TABLE "master.dbo".syslanguages (
  langid tsql_smallint,
  dateformat tsql_nvarchar(6),
  datefirst tsql_tinyint,
  upgrade tsql_int,
  name tsql_nvarchar(256),
  alias tsql_nvarchar(256),
  months tsql_nvarchar(744),
  shortmonths tsql_nvarchar(264),
  days tsql_nvarchar(434),
  lcid tsql_int,
  msglangid tsql_smallint
);
INSERT INTO "master.dbo".syslanguages VALUES (0, 'mdy', 7, 0, 'us_english', 'English', 'January,February,March,April,May,June,July,August,September,October,November,December',
  'Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec', 'Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,Sunday', 1033, 1033);

DROP TABLE IF EXISTS "master.sys".configurations;
CREATE TABLE "master.sys".configurations (
  configuration_id tsql_int NOT NULL,
  name tsql_nvarchar(70),
  value tsql_int, --tsql_sql_variant(8016),
  minimum tsql_int, --tsql_sql_variant(8016),
  maximum tsql_int, --tsql_sql_variant(8016),
  value_in_use tsql_int, --tsql_sql_variant(8016),
  description tsql_nvarchar(510),
  is_dynamic tsql_bit,
  is_advanced tsql_bit
);
INSERT INTO "master.sys".configurations VALUES (16387, 'SMO and DMO XPs', 1, 0, 1, 1, 'Enable or disable SMO and DMO XPs', 1, 1);

DROP VIEW IF EXISTS "master.sys".databases CASCADE;
CREATE VIEW "master.sys".databases AS
SELECT substring(nspname from '^[^.]*')::tsql_sysname "name",
 oid::tsql_int database_id,
 NULL::tsql_int source_database_id,
 'Latin1_General_CI_AS'::tsql_sysname AS collation_name,
 0::tsql_tinyint state
FROM pg_namespace
WHERE nspname LIKE '%.dbo';

DROP VIEW IF EXISTS "master.dbo".sysdatabases;
CREATE VIEW "master.dbo".sysdatabases AS
SELECT name,
database_id dbid
FROM "master.sys".databases;

DROP VIEW IF EXISTS "master.dbo".syslogins;
CREATE VIEW "master.dbo".syslogins AS SELECT usename::tsql_nvarchar "name", 'master'::tsql_nvarchar "dbname", ''::tsql_nvarchar "password", usename::tsql_nvarchar "loginname" FROM pg_user;

DROP VIEW IF EXISTS "master.dbo".sysusers;
CREATE VIEW "master.dbo".sysusers AS
SELECT "substring"(pg_user.usename::text, '[^.]*$'::text)::tsql_nvarchar AS name
FROM pg_user
WHERE "substring"(pg_user.usename::text, '^[^.]*.'::text)::tsql_nvarchar = 'master.'::tsql_nvarchar;

DROP VIEW IF EXISTS "master.dbo".sysdatabases;
CREATE VIEW "master.dbo".sysdatabases AS SELECT DISTINCT substring(nspname from '^[^.]*')::tsql_nvarchar "name" FROM pg_namespace;
