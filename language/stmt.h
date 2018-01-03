#ifndef _STMT_H
#define _STMT_H

#include <string>
#include <vector>
#include <map>

#include "exec.h"

using namespace std;

class tsql_stmt {
private:
public:
  vector<string> vars;
  tsql_stmt() {}
  //virtual const char *c_str() = 0;
  virtual int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) = 0;
};

#endif
