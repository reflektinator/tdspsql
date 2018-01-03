#ifndef _STMT_DROP_DATABASE_H
#define _STMT_DROP_DATABASE_H

#include "stmt.h"

class tsql_stmt_drop_database : public tsql_stmt {
public:
  char *database;

  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    vector<string> vars; /* empty */
    char *query;
    tsql_results *results;

    asprintf(&query, "SELECT nspname FROM pg_namespace WHERE nspname LIKE '%s%%'", database);
    results = exec->exec_int(query, vars, prog_vars);
    if (results->is_error()) {
      exec->error(1, 1, 16, "internal error 201403011956");
      free(query);
      return 1;
    }
    free(query);
    for (int i = 0; i < results->row_count(); i++) {
      asprintf(&query, "DROP SCHEMA \"%s\" CASCADE", results->field_value(i, 0));
      if (exec->exec_cmd(query, vars, prog_vars)) {
        exec->error(1, 1, 16, "internal error 201403022110");
printf("%s\n", query);
        free(query);
        return 1;
      }
    }
    return 0;
  }
};

#endif
