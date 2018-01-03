#ifndef _STMT_EXEC_H
#define _STMT_EXEC_H

#include <string.h>
#include <endian.h>
#include "stmt.h"
#include "program.h"

tsql_program *tsql_parse(const char *query);

class tsql_stmt_exec : public tsql_stmt {
public:
  //virtual int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) = 0;
  int do_exec(string sql, tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    pprintf(WARNING, "executing text %s", sql.c_str());
    tsql_program *program = tsql_parse(sql.c_str());
    if (program->error_message) {
      pprintf(WARNING, "unreported error %s", program->error_message);
      return 1;
    }
    program->exec(exec, prog_vars);
    return 0;
  }
};

#define PROC_PARAM_TYPE_NULL     0
#define PROC_PARAM_TYPE_VAR      1
#define PROC_PARAM_TYPE_GLOBAL   2
#define PROC_PARAM_TYPE_VARCHAR  3
#define PROC_PARAM_TYPE_NVARCHAR 4
#define PROC_PARAM_TYPE_INT      5

struct proc_param_t {
  int type;
  bool output;
  char *value;
};

class tsql_stmt_exec_proc : public tsql_stmt_exec {
public:
  string proc;
  vector<proc_param_t> *params;
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    map<string, tsql_var> proc_vars;
    tsql_results *results;
    map<string, tsql_var> local_prog_vars;
    vector<string> local_vars;
    local_prog_vars["@proc"] = tsql_var_varchar(proc);
    local_vars.push_back("@proc");
    pprintf(WARNING, "exec %s", proc.c_str());
    results = exec->exec_int("SELECT pg_proc.prosrc, pg_proc.pronargs FROM pg_proc JOIN pg_language ON pg_proc.prolang = pg_language.oid WHERE pg_language.lanname = 'tsqlproc' AND pg_proc.oid = $1::varchar::regproc::oid", local_vars, local_prog_vars);
    if (results->is_error() || results->row_count() == 0) {
      char *error_message;
      asprintf(&error_message, "Could not find stored procedure '%s'.", proc.c_str());
      exec->error(2812, 62, 16, error_message);
      free(error_message);
      delete results;
      return 1;
    }
    char *proc_src = results->field_value(0, 0);
    pprintf(WARNING, "prosrc(1) = %.80s", proc_src);
    int arg_count = *(int16_t *)results->field_value(0, 1);
    pprintf(WARNING, "prosrc(1) = %.80s", proc_src);
    pprintf(WARNING, "pronargs = %d", arg_count);
    // TODO error if more than 1 row
    // TODO check that pronargs matches values... accounting for default parameters
    delete results;
//resolve the expression
//if the expression is only an existing param then a shortcut can be taken
    if (arg_count > 0) {
#if 0
      results = exec->exec_int(
       "SELECT UNNEST(pg_proc.proargnames), UNNEST(pg_proc.proallargtypes), UNNEST(pg_proc.proargmodes)"
       " FROM pg_proc"
       " JOIN pg_language"
       " ON pg_proc.prolang = pg_language.oid"
       " WHERE pg_language.lanname = 'tsqlproc'"
       " AND pg_proc.oid = $1::varchar::regproc::oid", local_vars, local_prog_vars);
       if (results->is_error()) {
        exec->error(1, 1, 16, "201403031757");
        delete results;
        return 1;
      }
      char *sql;
      const char *prefix = "SELECT ";
      sql = strdup("");
pprintf(WARNING, "aaa - params.size() = %ld", (*params).size());
pprintf(WARNING, "aaa - resuls->row_count()() = %d", results->row_count());
      for (int i = 0; i < results->row_count(); i++) {
        pprintf(WARNING, "proargnames[%d] = %s", i, results->field_value(i, 0));
        pprintf(WARNING, "proargtypes[%d] = %d", i, (int)(intptr_t)results->field_value(i, 1));
        if (strcmp(results->field_value(i, 2), "i") == 0) {
          /* parameter is input - specify in sql string and add to parameter list */
          char *newsql;
          if ((*params)[param].value[0] == '@') {
            asprintf(&newsql, "%s%s$%d", sql, prefix, varnum);
            // add parameter here...
          } else {
            asprintf(&newsql, "%s%s%s", sql, prefix, (*params)[param].value);
          }
pprintf(WARNING, "ddd - %s", newsql);
          prefix = ", ";
          free(sql);
          sql = newsql;
          param++;
        }
      }
pprintf(WARNING, "final sql = %s", sql);
      tsql_results *results2;
      results2 = exec->exec_int(sql, vars, prog_vars);
      if (results2->is_error()) {
        exec->error(1, 1, 16, "201403031734");
        return 1;
      }
      // TODO also initialise defaults...
      for (int i = 0; i < (int)results->row_count(); i++) {
        if (results->field_value(i, 2)[0] == 'o') {
pprintf(WARNING, "skipping output parameter");
          continue;
        }
        //printf("%s = %s", results->field_value(i, 0), results2->field_value(0, i));
        proc_vars[results->field_value(i, 0)] = tsql_var(*results2, 0, i);
      }
      param++;
#endif
    }
    return do_exec(proc_src, exec, proc_vars);
  }
};

#if 0
class tsql_stmt_exec_var : public tsql_stmt_exec {
public:
  string var;
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    string proc_src = string((char *)prog_vars[var].value, prog_vars[var].length);
    return do_exec(proc_src, exec, prog_vars);
  }
};
#endif

class tsql_stmt_exec_expr : public tsql_stmt_exec {
public:
  string expr;
  int exec(tsql_executor *exec, map<string, tsql_var> &prog_vars) {
    char *sql;
    tsql_results *results;

    asprintf(&sql, "SELECT %s", expr.c_str());
    results = exec->exec_int(sql, vars, prog_vars);
    if (results->is_error() || results->row_count() != 1 || results->col_count() != 1) {
      // TODO also ensure that field(0, 0) is char, varchar, nchar, nvarchar, text, or ntext
      char *error_message;
      asprintf(&error_message, "201403091039: %s", results->error_message());
      exec->error(2812, 62, 16, error_message);
      free(error_message);
      delete results;
      return 1;
    }
    sql = (char *)malloc(results->field_length(0, 0) + 1);
    memcpy(sql, results->field_value(0, 0), results->field_length(0, 0));
    sql[results->field_length(0, 0)] = 0;
    delete results;
    
    int ret = do_exec(sql, exec, prog_vars);
    free(sql);
    return ret;
  }
};

#endif
