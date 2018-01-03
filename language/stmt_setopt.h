#ifndef _STMT_SETOPT_H
#define _STMT_SETOPT_H

#include "stmt.h"

#define is_on(v) ((strcasecmp(v, "ON")==0)?1:0)

class tsql_stmt_setopt : public tsql_stmt {
  string option;
  string value;
public:
  //const char *c_str() { return expr.c_str(); }
  //void sprintf(const char *fmt, ...) { char *tmp; va_list va; va_start(va, fmt); vasprintf(&tmp, fmt, va); va_end(va); expr = tmp; free(tmp);}
  void set_option(const char *v) { option = v; }
  void set_value(const char *v) { option = v; }

  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
pprintf(WARNING, "executing SET %s %s\n", option.c_str(), value.c_str());
    if (strcasecmp(value.c_str(), "fmtonly")) {
      // TODO also reject if value != OFF
      exec->fmtonly = is_on(value.c_str());
    } else if (strcasecmp(value.c_str(), "nocount")) {
      exec->nocount = is_on(value.c_str());
    } else {
      // TODO this is an error
    }
    return 0;
  }
};

#endif
