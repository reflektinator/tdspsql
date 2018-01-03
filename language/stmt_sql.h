#ifndef _STMT_SQL_H
#define _STMT_SQL_H

#include "stmt.h"

class tsql_stmt_sql : public tsql_stmt {
public:
  string sql;
  const char *c_str() { return sql.c_str(); }
  void sprintf(const char *fmt, ...) { char *tmp; va_list va; va_start(va, fmt); vasprintf(&tmp, fmt, va); va_end(va); sql = tmp; free(tmp);}
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    return exec->exec_ext(sql, vars, prog_vars);
  }
};

#endif
