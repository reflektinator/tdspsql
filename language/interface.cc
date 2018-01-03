#include "parser.h"
#include "executor.h"
#include "parse.y.h"
#include "scan.l.h"

tsql_program *
tsql_parse(const char *query, tsql_types *types) {
  YY_BUFFER_STATE buffer_state;
  bison_args args;

  args.program = new tsql_program();
  args.types = types;

  yylex_init(&args.scanner);
  buffer_state = yy_scan_string(query, args.scanner);
  yyparse(&args);
  yy_delete_buffer(buffer_state, args.scanner);
  yylex_destroy(args.scanner);
  return args.program;
}
