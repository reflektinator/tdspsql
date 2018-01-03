#ifndef _STMT_ASSIGN_H
#define _STMT_ASSIGN_H

#include "stmt.h"

using namespace std;

class tsql_assign_column {
public:
  char *expr;
  vector<string> vars;
};

class tsql_stmt_assign_select : public tsql_stmt {
  string sql;
public:
  vector<string> assign_vars;
  const char *c_str() { return sql.c_str(); }
  void sprintf(const char *fmt, ...) { char *tmp; va_list va; va_start(va, fmt); vasprintf(&tmp, fmt, va); va_end(va); sql = tmp; free(tmp);}
  //void set_var(const char *var) { this->var = var; }
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    tsql_results *results;
    results = exec->exec_int(sql, vars, prog_vars);
    if (results->is_error()) {
      char *msg;
      asprintf(&msg, "201403091906: %s", results->error_message());
      exec->error(1, 1, 16, msg);
      free(msg);
      return 1;
    }
    if (results->row_count() != 1) {
      exec->error(1, 1, 16, "201403091907");
      return 1;
    }
    for (int i = 0; i < results->col_count(); i++) {
      prog_vars[assign_vars[i]] = tsql_var(*results, 0, 0);
    }
    return 0;
  }
};

#endif
