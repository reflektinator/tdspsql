#ifndef _PROGRAM_H
#define _PROGRAM_H

#include <string>
#include <set>
#include "stmt.h"
#include "exec.h"

class tsql_program {
public:
  char *error_message = NULL;
  int first_line;
  int last_line;
  tsql_stmt_block statements;
  map<string, tsql_var> vars;
  void add_statement(tsql_stmt *stmt) { statements.statements.push_back(stmt); }
  void add_var(string name, int type) { pprintf(WARNING, "add_var %s", name.c_str()); vars[name] = tsql_var(type); pprintf(WARNING, "added var"); }
  int exec(tsql_executor *exec) {
    set<string>::iterator i;
    map<string, tsql_var> prog_vars;
    return statements.exec(exec, prog_vars);
  }
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    return statements.exec(exec, prog_vars);
  }
};

#endif
