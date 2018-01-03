#ifndef _STMT_EMPTY_H
#define _STMT_EMPTY_H

#if 0
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <list>
#include <vector>
#include <set>
#endif

#include "stmt.h"

using namespace std;

class tsql_stmt_empty : public tsql_stmt {
private:
public:
  tsql_stmt_empty() {}
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    return exec->exec_nul();
  }
};

#endif
