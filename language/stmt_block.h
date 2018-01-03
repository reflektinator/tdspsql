#ifndef _STMT_BLOCK_H
#define _STMT_BLOCK_H

#include "stmt.h"

using namespace std;

class tsql_stmt_block : public tsql_stmt {
public:
  vector<tsql_stmt *> statements;
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    for (int i = 0; i < (int)statements.size(); i++) {
      tsql_stmt *s = statements[i];
      if (s->exec(exec, prog_vars)) {
#if 0
        char *msg;
        asprintf(&msg, "201403091425: %s\n", "unknown");
        exec->error(1, 1, 16, msg);
        free(msg);
#endif
        return 1;
      }
    }
    return 0;
  }
};

#endif
