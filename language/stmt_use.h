#ifndef _STMT_USE_H
#define _STMT_USE_H

#include "stmt.h"

class tsql_stmt_use : public tsql_stmt {
public:
  char *database;

  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    char *query;
    char *schemaname;
    tsql_results *results;
    vector<string> vars; /* empty */
    asprintf(&schemaname, "%s.%s", database, "dbo");
    // TODO check that user has permission to access this "database"
    asprintf(&query, "SELECT COUNT(*) FROM pg_namespace WHERE nspname = '%s'", schemaname);
    results = exec->exec_int(query, vars, prog_vars);
    if (results->is_error()) {
      exec->error(1, 1, 16, "internal error 201403011954");
      delete results;
      free(query);
      free(schemaname);
      return 1;
    }
printf("row_count = %d\n", results->row_count());
printf("result = %d, length = %d\n", *(int32_t *)results->field_value(0, 0), results->field_length(0, 0));
    if (be64toh(*(uint64_t *)results->field_value(0, 0)) == 0) {
      char *error_message;
      asprintf(&error_message, "Could not locate entry in sysdatabases for database '%s'. No entry found with that name. Make sure that the name is entered correctly.", database);
      exec->error(911, 1, 16, error_message);
      delete results;
      free(query);
      free(error_message);
      free(schemaname);
      return 1;
    }
    delete results;
    free(query);
    asprintf(&query, "SET SEARCH_PATH TO \"%s\", \"master.dbo\", public", schemaname);
    if (exec->exec_cmd(query, vars, prog_vars)) {
      exec->error(1, 1, 16, "internal error 201403011955");
      free(query);
      free(schemaname);
      return 1;
    }
    exec->set_database(database);
    free(query);
    free(schemaname);
    return 0;
  }
};

#endif
