#ifndef _STMT_CREATE_DATABASE_H
#define _STMT_CREATE_DATABASE_H

#include "stmt.h"

using namespace std;

class tsql_stmt_create_database : public tsql_stmt {
public:
  char *database;

  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    char *query;
    vector<string> vars; /* empty */
    asprintf(&query, "CREATE SCHEMA \"%s.dbo\"", database);
    exec->exec_cmd(query, vars, prog_vars);
    asprintf(&query, "CREATE SCHEMA \"%s.information_schema\"", database);
    exec->exec_cmd(query, vars, prog_vars);
    asprintf(&query,
      "CREATE VIEW \"%s.information_schema\".tables AS"
      " SELECT substring(schemaname from '^[^.]*')::tsql_varchar tables_catalog,"
      " substring(schemaname from '[^.]*$')::tsql_varchar table_schema,"
      " tablename::tsql_varchar table_name,"
      " 'BASE TABLE'::tsql_varchar table_type"
      " FROM pg_tables"
      " WHERE substring(schemaname from '^[^.]*.') = '%s.'", database, database);
    exec->exec_cmd(query, vars, prog_vars);
    asprintf(&query,
      "CREATE VIEW \"%s.dbo\".sysusers AS"
      " SELECT substring(usename from '[^.]*$')::tsql_varchar \"name\""
      " FROM pg_user"
      " WHERE substring(usename from '^[^.]*.')::tsql_varchar = '%s.'::tsql_varchar", database, database);
    exec->exec_cmd(query, vars, prog_vars);
    asprintf(&query,
      "CREATE VIEW \"%s.dbo\".sysobjects AS"
      " SELECT pg_class.relname::tsql_varchar(128) \"name\","
      "  pg_class.oid::tsql_int \"id\","
      "  CASE"
      "   WHEN pg_class.relkind = 'r' THEN 'U'::tsql_varchar(2)"
      "   WHEN pg_class.relkind = 'v' THEN 'V'::tsql_varchar(2)"
      "   WHEN pg_class.relkind = 'i' AND pg_index.indisprimary THEN 'PK'::tsql_varchar(2)"
      "   WHEN pg_class.relkind = 'i' AND pg_index.indisunique THEN 'UQ'::tsql_varchar(2)"
      "   WHEN pg_class.relkind = 'i' THEN 'II'::tsql_varchar(2)"
      "  END \"xtype\","
      "  '1'::tsql_smallint \"uid\","
      "  COALESCE(pg_index.indrelid::tsql_int, '0'::tsql_int) \"parent_obj\","
      "  CASE"
      "   WHEN pg_class.relkind = 'r' THEN 'U'::tsql_varchar(1)"
      "   WHEN pg_class.relkind = 'v' THEN 'V'::tsql_varchar(1)"
      "   WHEN pg_class.relkind = 'i' THEN 'K'::tsql_varchar(1)"
      "  END \"type\""
      " FROM pg_class"
      " JOIN pg_namespace"
      " ON pg_class.relnamespace = pg_namespace.oid"
      " LEFT OUTER JOIN pg_index"
      " ON pg_class.oid = pg_index.indexrelid"
      " WHERE substring(pg_namespace.nspname from '^[^.]*.')::tsql_varchar = '%s.'::tsql_varchar"
      " AND pg_class.relkind IN ('r', 'i', 'v')"
      " AND (pg_index.indexrelid IS NULL OR pg_index.indisprimary OR pg_index.indisunique)", database, database);
pprintf(WARNING, "%s\n", query);
    exec->exec_cmd(query, vars, prog_vars);
    asprintf(&query,
      "CREATE VIEW \"%s.dbo\".syscolumns AS"
      " SELECT attname::tsql_varchar(128) \"name\","
      " attrelid::tsql_int id,"
      " 0::tsql_tinyint xtype,"
      " 0::tsql_tinyint xusertype,"
      " attlen::int4::tsql_smallint length,"
      " attnum::int4::tsql_smallint colid,"
      " attnum::int4::tsql_smallint colorder,"
      " pg_collation.oid::tsql_int collationid,"
      " collname::tsql_varchar(128) \"collation\","
      " ''::tsql_varbinary(5) tdscollation"
      " FROM pg_attribute"
      " JOIN pg_class"
      " ON attrelid = pg_class.oid"
      " JOIN pg_namespace"
      " ON relnamespace = pg_namespace.oid"
      " LEFT JOIN pg_collation"
      " ON attcollation = pg_collation.oid"
      " WHERE substring(nspname from '^[^.]*.')::tsql_varchar = '%s.'::tsql_varchar"
      " AND attnum > 0", database, database);
    exec->exec_cmd(query, vars, prog_vars);
    asprintf(&query,
      "CREATE VIEW \"%s.dbo\".sysindexes AS"
      " SELECT pg_class.relname::tsql_varchar(128) \"name\","
      "  pg_class.oid::tsql_int \"id\""
      " FROM pg_class"
      " JOIN pg_namespace"
      " ON relnamespace = pg_namespace.oid"
      " JOIN pg_index"
      " ON pg_class.oid = pg_index.indexrelid"
      " WHERE substring(pg_namespace.nspname from '^[^.]*.')::tsql_varchar = '%s.'::tsql_varchar", database, database);
    exec->exec_cmd(query, vars, prog_vars);

//sp_tablecollations
//sp_bcp_dbcmptlevel
    return 0;
  }
};

#endif
