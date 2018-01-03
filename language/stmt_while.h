#ifndef _STMT_WHILE_H
#define _STMT_WHILE_H

#include "stmt.h"

class tsql_stmt_while : public tsql_stmt {
public:
  char *expr;
  tsql_stmt *stmt;
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    int result;
printf("aaa var count = %ld\n", vars.size());
    if (exec->exec_bool(expr, vars, prog_vars, &result)) {
      exec->error(1, 1, 16, "internal error 201403011951");
      return 1;
    }
    while (result) {
      if (stmt->exec(exec, prog_vars)) {
        exec->error(1, 1, 16, "internal error 201403011952");
        return 1;
      }
      if (exec->exec_bool(expr, vars, prog_vars, &result)) {
        exec->error(1, 1, 16, "internal error 201403011953");
        return 1;
      }
    }
    return 0;
  }
};

#endif
