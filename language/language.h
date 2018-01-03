#ifndef _LANGUAGE_H
#define _LANGUAGE_H

#include <string>
#include <vector>

#include "misc.h"
#include "frag.h"
#include "stmt.h"
#include "stmt_assign_select.h"
#include "stmt_block.h"
#include "stmt_create_database.h"
#include "stmt_drop_database.h"
#include "stmt_empty.h"
#include "stmt_exec.h"
#include "stmt_fetch.h"
#include "stmt_if.h"
#include "stmt_print.h"
#include "stmt_set.h"
#include "stmt_setopt.h"
#include "stmt_sql.h"
#include "stmt_use.h"
#include "stmt_while.h"
#include "program.h"
#include "exec.h"

class tsql_program;

using namespace std;

struct bison_args {
  tsql_types *types;
  vector<string> stmt_vars;
  void *scanner;
  tsql_program *program;
};

//tsql_program *tsql_parse(const char *query, tsql_types *types);

#endif

