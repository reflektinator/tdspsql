#ifndef _STMT_PRINT_H
#define _STMT_PRINT_H

#include "stmt.h"

class tsql_stmt_print : public tsql_stmt {
public:
  char *expr;
  tsql_stmt *true_block;
  tsql_stmt *false_block;
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    char *query;
    tsql_results *results;
    
    asprintf(&query, "SELECT CAST(%s AS tsql_varchar)", expr);
    results = exec->exec_int(query, vars, prog_vars);
    if (results->is_error()) {
      char *error_message;
      asprintf(&error_message, "201403062152: %s", results->error_message());
      exec->error(2812, 62, 16, error_message);
      free(error_message);
      delete results;
      return 1;
    }
    exec->info(0, 1, 0, results->field_value(0, 0));
    delete results;
    return 0;
  }
};

#endif
