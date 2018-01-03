#ifndef _STMT_IF_H
#define _STMT_IF_H

#include "stmt.h"

class tsql_stmt_if : public tsql_stmt {
public:
  char *expr;
  tsql_stmt *true_block;
  tsql_stmt *false_block;
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    int result;
pprintf(WARNING, "if stmt");
    if (exec->exec_bool(expr, vars, prog_vars, &result)) {
pprintf(WARNING, "is error");
      exec->error(1, 1, 16, "internal error 201403011950");
      return 1;
    }
    if (result) {
pprintf(WARNING, "is true");
      return true_block->exec(exec, prog_vars);
    } else if (false_block) {
pprintf(WARNING, "is false");
      return false_block->exec(exec, prog_vars);
    }
pprintf(WARNING, "is false (but no false block)");
    return 0;
  }
};

#endif
