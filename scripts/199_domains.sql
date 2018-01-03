-- not really right
CREATE DOMAIN tsql_timestamp timestamp;
-- will be tsql_varbinary(max)
CREATE DOMAIN tsql_image bytea;
CREATE DOMAIN tsql_money tsql_decimal(19,6);

CREATE DOMAIN tsql_sysname tsql_nvarchar(128);
