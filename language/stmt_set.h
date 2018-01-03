#ifndef _STMT_SET_H
#define _STMT_SET_H

#include "../tdspsql.h"

class tsql_stmt_set : public tsql_stmt {
  string expr;
  string var;
public:
  const char *c_str() { return expr.c_str(); }
  void sprintf(const char *fmt, ...) { char *tmp; va_list va; va_start(va, fmt); vasprintf(&tmp, fmt, va); va_end(va); expr = tmp; free(tmp);}
  void set_var(const char *var) { this->var = var; }
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
// TODO - check that type matches, or alternatively cast to target type and let psql fail it
pprintf(WARNING, "executing set\n");
    tsql_results *results;
    results = exec->exec_int(expr, vars, prog_vars);
    if (results->is_error()) {
      char *msg;
      asprintf(&msg, "20140311946: %s\n", results->error_message());
      exec->error(1, 1, 16, msg);
      free(msg);
      return 1;
    }
//pprintf(WARNING, "setting %s to %s\n", var.c_str(), results->field_value(0, 0));
    prog_vars[var] = tsql_var(*results, 0, 0);
    delete results;
    return 0;
  }
};

#endif

