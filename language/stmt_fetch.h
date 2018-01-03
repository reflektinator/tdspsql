#ifndef _STMT_FETCH_H
#define _STMT_FETCH_H

#include "stmt.h"

using namespace std;

class tsql_stmt_fetch : public tsql_stmt {
public:
  string cursor;
  vector<string> *params;
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    vector<string> empty;
    tsql_results *results;
    char *sql;
    //int fetch;
    asprintf(&sql, "FETCH NEXT FROM %s\n", cursor.c_str());
    results = exec->exec_int(sql, empty, prog_vars);
    if (results->is_error()) {
      exec->error(1, 1, 16, "internal error 201403071334");
      return 1;
    }
    if (results->row_count() == 0) {
printf("no more data from cursor\n");
      exec->sess_vars["@@fetch_status"] = tsql_var_int(-1);
      return 0;
    }
    exec->sess_vars["@@fetch_status"] = tsql_var_int(0);
printf("params.size() = %ld\n", params->size());
printf("results->col_count() = %d\n", results->col_count());
    for (int i = 0; i < (int)(((int)params->size() > results->col_count())?results->col_count():params->size()); i++) {
printf("i = %d\n", i);
printf("var = %s\n", (*params)[i].c_str());
      prog_vars[(*params)[i]] = tsql_var(*results, 0, i);
    }
    // TODO report unused results or unfilled parameters
    delete results;
    return 0;
  }
};

#endif
