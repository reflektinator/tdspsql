%pure-parser
%error-verbose
%parse-param { struct bison_args *args }
%lex-param { void *scanner }
%defines "parse.y.h"
%output "parse.y.cc"
%locations

%code top {
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include "language.h"
#include "parse.y.h"
#define YY_HEADER_EXPORT_START_CONDITIONS
#include "scan.l.h"

void
make_lower(char *s) {
  for (int i = 0; s[i]; i++) {
    s[i] = tolower(s[i]);
  }
}

tsql_program *
tsql_parse(const char *query) {
  YY_BUFFER_STATE buffer_state;
  bison_args args;

  args.program = new tsql_program();

  yylex_init(&args.scanner);
  buffer_state = yy_scan_string(query, args.scanner);
  yyparse(&args);
  yy_delete_buffer(buffer_state, args.scanner);
  yylex_destroy(args.scanner);
  return args.program;
}

}

%code requires {
void set_scan_state(bison_args *, int state);

struct param_defs_t {
  vector<string> *param_list;
  char *type_defs;
  char *name_type_defs;
};

struct column_defs_t {
  char *name_type_defs;
  char *full_defs;
};

}

%union {
  char                      *strval;
  int                       on_off;
  tsql_frag                 *frag;
  tsql_stmt                 *stmt;
  tsql_stmt_assign_select   *stmt_assign_select;
  tsql_assign_column        *assign_column;
  tsql_stmt_create_database *stmt_create_database;
  //tsql_stmt_alter_database  *stmt_alter_database;
  tsql_stmt_drop_database   *stmt_drop_database;
  tsql_stmt_empty           *stmt_empty;
  tsql_stmt_if              *stmt_if;
  tsql_stmt_while           *stmt_while;
  tsql_stmt_exec            *stmt_exec;
  tsql_stmt_fetch           *stmt_fetch;
  tsql_stmt_block           *stmt_block;
  tsql_stmt_sql             *stmt_sql;
  tsql_stmt_use             *stmt_use;
  tsql_stmt_print           *stmt_print;

  vector<string>            *vector_string;
  param_defs_t              param_defs;
  vector<proc_param_t>      *vector_proc_param;
  proc_param_t              proc_param;
  struct column_defs_t column_defs;
  //uint64_t intval;
  //double   floatval;
}

%{

//int yylex(YYSTYPE *yylval, void *scanner);

void
yyerror(YYLTYPE *yylloc, bison_args *args, char const *error_message) {
  printf("%s\nyylloc->first_line = %d, yylloc->last_line = %d\n", error_message, yylloc->first_line, yylloc->last_line);
  args->program->error_message = strdup(error_message);
  args->program->first_line = yylloc->first_line;
  args->program->last_line = yylloc->last_line;
}

#define scanner args->scanner

%}

%token <strval> vDECIMAL
%token <strval> vIDENTIFIER
%token <strval> vINTEGER
%token <strval> vLITERAL
%token <strval> vNLITERAL
%token <strval> vNAME
%token <strval> vTEMPTABLE
%token <strval> vVAR
%token <strval> vFUNCTION


/* keywords and other reserved words */
%token ADD ALL ALTER AND AS ASC
%token _BEGIN BIT BY
%token CASE CAST CHAR CREATE CROSS CLUSTERED COALESCE CONSTRAINT CONVERT COMMITTED
%token COUNT CURSOR COLLATE
%token DATABASE DATEPART DB_NAME DEALLOCATE DECLARE DEFAULT DELETE DESC DISTINCT DROP
%token ELSE END EXEC EXECUTE EXISTS
%token FETCH FLOAT FOR FOREIGN FROM FUNCTION
%token HAVING
%token GRANT GROUP
%token IDENTITY IF IN INTO INDEX INNER INSERT IS ISOLATION
%token IMAGE IMMEDIATE
%token JOIN
%token KEY
%token LEFT LEVEL LIKE LOGIN
%token NEWID NEXT NONCLUSTERED NOT NTEXT _NULL NUMERIC
%token OFF ON OPEN OR ORDER OUTER OUTPUT
%token PASSWORD PRIMARY PRINT PROCEDURE
%token READ REAL REFERENCES RETURNS ROLLBACK
%token RECURSIVE_TRIGGERS
%token SELECT SERVER SET SINGLE_USER
%token TABLE TEXT THEN TO TOP TRANSACTION
%token UNION UNIQUE UNIQUEIDENTIFIER UPDATE USE USER
%token VALUES VARBINARY VARCHAR VIEW
%token WHEN WHERE WHILE WITH

%type <on_off> on_off

%type <strval> constraint
%type <strval> default_spec
%type <strval> field_list
%type <strval> field_name
%type <strval> field_name_list
%type <strval> order_by_field_list
%type <strval> order_by_field
%type <strval> from_source
%type <strval> function_text
%type <strval> identifier
%type <strval> identity_spec
%type <strval> index_modifier
%type <strval> index_modifiers
%type <strval> join_type_with_on
%type <strval> join_type_without_on
%type <strval> key_spec
%type <strval> column_modifier
%type <strval> column_modifiers
%type <strval> null_spec
%type <strval> privilege
%type <strval> proc_name
%type <strval> table_name
%type <strval> type
%type <strval> type_modifier
%type <strval> type_modifiers
%type <strval> type_name

%type <frag> alter_table
%type <frag> case
%type <frag> column
%type <frag> columns
%type <frag> condition
%type <frag> deallocate
%type <frag> declare_cursor
%type <frag> create_function
%type <frag> create_index
%type <frag> create_login
%type <frag> create_procedure
%type <frag> create_table
%type <frag> create_user
%type <frag> create_view
%type <frag> drop_table
%type <frag> expr
%type <frag> fields_clause
%type <frag> from_clause
%type <frag> grant
%type <frag> group_by_clause
%type <frag> insert
%type <frag> join_types
%type <frag> joins
%type <frag> order_by_clause
%type <frag> params
%type <proc_param> proc_param
%type <frag> select_frag
%type <frag> top_clause
%type <frag> union
%type <frag> when1
%type <frag> when1_list
%type <frag> when2
%type <frag> when2_list
%type <frag> where_clause

%type <stmt_sql> sql

%type <stmt_if> _if

%type <stmt_while> _while

%type <stmt_exec> exec

%type <stmt_assign_select>   assign_select
%type <assign_column>        assign_column
%type <assign_column>        assign_columns
%type <stmt_create_database> create_database
%type <stmt_empty>           alter_database
%type <stmt_drop_database>   drop_database
%type <stmt_empty>           declare
%type <stmt_empty>           open
%type <stmt_fetch>           fetch
%type <stmt_block>           block
%type <stmt_block>           statements
%type <stmt_print>           print
/* set could be one of a few things */
%type <stmt> set
%type <stmt> statement
%type <stmt_use> use

%type <vector_proc_param> proc_params
%type <vector_string> fetch_vars

%type <param_defs> param_def
%type <param_defs> param_defs

%type <column_defs> column_def
%type <column_defs> column_defs

%left IF
%left ELSE
%left '+' '-'
%left '*' '/'
%left '<' '>'
%left '&' '|'
%left AND OR
%left NOT
%left '.'
%left '(' ')'

%%

program:
  /* empty */ {}
  | proc_name proc_params {
      tsql_stmt_exec_proc *stmt = new tsql_stmt_exec_proc();
      stmt->proc = $1;
      stmt->params = $2;
      args->program->statements.statements.push_back(stmt); }
  | proc_name {
      tsql_stmt_exec_proc *stmt = new tsql_stmt_exec_proc();
      stmt->proc = $1;
      stmt->params = NULL;
      args->program->statements.statements.push_back(stmt); }
  | statements { args->program->statements.statements.push_back($1); }

statements:
  statement { $$ = new tsql_stmt_block();  $$->statements.push_back($1); }
  | statements statement { $$ = $1; $$->statements.push_back($2); }

statement:
  declare { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | set { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | use { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | create_database { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | alter_database { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | drop_database { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | assign_select { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | sql { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | _if { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | _while { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | exec { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | print { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); } 
  | fetch { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | block { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }
  | open { $$ = $1; $$->vars = args->stmt_vars; args->stmt_vars.clear(); }

block:
  _BEGIN statements END { $$ = $2; }

fetch:
  FETCH NEXT FROM vNAME INTO fetch_vars {
    $$ = new tsql_stmt_fetch();
    $$->cursor = $4;
    $$->params = $6;
  }

print:
  PRINT expr { $$ = new tsql_stmt_print(); $$->expr = strdup($2->c_str()); }

_else:
  /* empty */ { $<stmt>$ = NULL; }
  | ELSE statement { $<stmt>$ = $2; }

_if:
  IF condition { $<vector_string>$ = new vector<string>; *$<vector_string>$ = args->stmt_vars; args->stmt_vars.clear(); } statement _else {
    $$ = new tsql_stmt_if();
    $$->expr = strdup($2->c_str());
    args->stmt_vars = *$<vector_string>3;
    delete $<vector_string>3;
    $$->true_block = $4;
    $$->false_block = $<stmt>5;
    delete $2; }
/*
  | IF condition { $<vector_string>$ = new vector<string>; *$<vector_string>$ = args->stmt_vars; args->stmt_vars.clear(); } statement ELSE statement {
    $$ = new tsql_stmt_if();
    $$->expr = strdup($2->c_str());
    args->stmt_vars = *$<vector_string>3;
    delete $<vector_string>3;
    $$->true_block = $4;
    $$->false_block = $6;
    delete $2; }
*/

_while:
  WHILE condition { $<vector_string>$ = new vector<string>; *$<vector_string>$ = args->stmt_vars; args->stmt_vars.clear(); } statement {
    $$ = new tsql_stmt_while();
    $$->expr = strdup($2->c_str());
    args->stmt_vars = *$<vector_string>3;
    delete $<vector_string>3;
    $$->stmt = $4;
    delete $2; }

exec:
  EXEC proc_name proc_params {
    $$ = new tsql_stmt_exec_proc();
    ((tsql_stmt_exec_proc *)$$)->proc = $2;
    ((tsql_stmt_exec_proc *)$$)->params = $3;
  }
  | EXEC proc_name {
    $$ = new tsql_stmt_exec_proc();
    ((tsql_stmt_exec_proc *)$$)->proc = $2;
    ((tsql_stmt_exec_proc *)$$)->params = new vector<proc_param_t>; }
//  | EXEC '(' vVAR ')' { make_lower($3); $$ = new tsql_stmt_exec_var(); ((tsql_stmt_exec_var *)$$)->var = $3; free($3); }
  | EXEC '(' expr ')' { $$ = new tsql_stmt_exec_expr(); ((tsql_stmt_exec_expr *)$$)->expr = $3->frag; delete $3; }

sql:
  union { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | insert { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | alter_table { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | create_view { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | create_index { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | create_function { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | create_procedure { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | create_table { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | drop_table { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | create_user { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | create_login { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | grant { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | declare_cursor { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }
  | deallocate { $$ = new tsql_stmt_sql(); $$->sql = $1->frag; }

declare_cursor:
  DECLARE vNAME CURSOR FOR union { $$ = new tsql_frag(); $$->sprintf("DECLARE %s BINARY CURSOR WITH HOLD FOR %s", $2, $5->c_str()); delete $2; delete $5; }

deallocate:
  DEALLOCATE vNAME { $$ = new tsql_frag(); $$->sprintf("CLOSE %s", $2); delete $2; } 

create_user:
  CREATE USER identifier { $$ = new tsql_frag(); $$->sprintf("CREATE USER \"##DATABASE##.%s\"", $3); delete $3; }

create_login:
  CREATE LOGIN identifier WITH { set_scan_state(args, with); } PASSWORD vLITERAL { set_scan_state(args, INITIAL); } {
    $$ = new tsql_frag();
    $$->sprintf("CREATE USER \"%s\" WITH PASSWORD '%s'", $3, $7);
    delete $3;
    delete $7; }

privilege:
  SELECT { $$ = strdup("SELECT"); }
  | INSERT { $$ = strdup("INSERT"); }
  | UPDATE { $$ = strdup("UPDATE"); }
  | DELETE { $$ = strdup("DELETE"); }
  | ALL { $$ = strdup("ALL"); }

grant:
  GRANT privilege ON table_name TO identifier { $$ = new tsql_frag(); $$->sprintf("GRANT %s ON %s TO %s", $2, $4, $6); free($2); free($4); free($6); }
  | GRANT EXECUTE ON table_name TO identifier { /* TODO */ $$ = new tsql_frag(); $$->frag = ""; free($4); }
  | GRANT VIEW SERVER vNAME TO identifier {
    if (strcasecmp($4, "state") != 0) {
      YYERROR;
    }
    $$ = new tsql_frag();
    $$->frag = "";
    free($6); }

params:
  expr { $$ = $1; }
  | params ',' expr { $$ = new tsql_frag(); $$->sprintf("%s, %s", $1->c_str(), $3->c_str()); delete $1; delete $3; }

proc_param:
  vLITERAL { $$.type = PROC_PARAM_TYPE_VARCHAR; $$.output = 0; $$.value = $1; }
  | vNLITERAL { $$.type = PROC_PARAM_TYPE_VARCHAR; $$.output = 0; $$.value = $1; }
  | vNAME { $$.type = PROC_PARAM_TYPE_VARCHAR; $$.output = 0; $$.value = $1; }
  | vIDENTIFIER { $$.type = PROC_PARAM_TYPE_VARCHAR; $$.output = 0; $$.value = $1; }
  | vINTEGER { $$.type = PROC_PARAM_TYPE_VARCHAR; $$.output = 0; $$.value = $1; }
  | vVAR { make_lower($1); $$.type = PROC_PARAM_TYPE_VARCHAR; $$.output = 0; $$.value = $1; }
  | vVAR OUTPUT { make_lower($1); $$.type = PROC_PARAM_TYPE_VARCHAR; $$.output = 1; $$.value = $1; }

proc_params:
  proc_param { $$ = new vector<proc_param_t>(); $$->push_back($1); }
  | proc_params ',' proc_param { $$ = $1; $$->push_back($3); }

fetch_vars:
  vVAR { make_lower($1); $$ = new vector<string>(); $$->push_back($1); delete $1; }
  | fetch_vars ',' vVAR { make_lower($3); $$ = $1; $$->push_back($3); delete $3; }

expr:
  '(' expr ')' { $$ = new tsql_frag(); $$->sprintf("(%s)", $2->c_str()); delete $2; }
  | '(' union ')' { $$ = new tsql_frag(); $$->sprintf("(%s)", $2->c_str()); delete $2; }
  | expr '-' expr { $$ = new tsql_frag(); $$->sprintf("%s - %s", $1->c_str(), $3->c_str()); delete $1; delete $3; } 
  | expr '+' expr { $$ = new tsql_frag(); $$->sprintf("%s + %s", $1->c_str(), $3->c_str()); delete $1; delete $3; } 
  | expr '*' expr { $$ = new tsql_frag(); $$->sprintf("%s * %s", $1->c_str(), $3->c_str()); delete $1; delete $3; } 
  | expr '/' expr { $$ = new tsql_frag(); $$->sprintf("%s / %s", $1->c_str(), $3->c_str()); delete $1; delete $3; } 
  | expr '&' expr { $$ = new tsql_frag(); $$->sprintf("%s & %s", $1->c_str(), $3->c_str()); delete $1; delete $3; } 
  | vINTEGER { $$ = new tsql_frag(); $$->sprintf("%s::tsql_int", $1); free($1); }
  | '-' vINTEGER { $$ = new tsql_frag(); $$->sprintf("-%s::tsql_int", $2); free($2); }
  | vDECIMAL { $$ = new tsql_frag(); $$->sprintf("%s::tsql_decimal", $1); free($1); }
  | '-' vDECIMAL { $$ = new tsql_frag(); $$->sprintf("-%s::tsql_decimal", $2); free($2); }
  | vLITERAL { $$ = new tsql_frag(); $$->sprintf("'%s'::tsql_varchar(%d)", $1, (int)strlen($1)); free($1); }
  | vNLITERAL { $$ = new tsql_frag(); $$->sprintf("'%s'::tsql_nvarchar(%d)", $1, (int)strlen($1)); free($1); }
  | _NULL { $$ = new tsql_frag(); $$->frag = "NULL::tsql_varchar"; } // TODO should be NULL::tsql_text when text type is created
  | field_name { $$ = new tsql_frag(); $$->sprintf("%s", $1); free($1); }
  | table_name '(' ')' { $$ = new tsql_frag(); $$->sprintf("%s()", $1); free($1); } /* function can be db.schema.function */
  | table_name '(' params ')' { $$ = new tsql_frag(); $$->sprintf("%s(%s)", $1, $3->c_str()); free($1); delete $3; } /* function can be db.schema.function */
  | COUNT '(' expr ')' { $$ = new tsql_frag(); $$->sprintf("count(%s)::tsql_bigint", $3->c_str()); delete $3; }
  | LEFT '(' params ')' { $$ = new tsql_frag(); $$->sprintf("left(%s)", $3->c_str()); delete $3; }
  | CAST '(' expr AS type_name ')' { $$ = new tsql_frag(); $$->sprintf("CAST(%s AS %s)", $3->c_str(), $5); delete $3; free($5); }
  | COALESCE '(' expr ',' expr ')' { $$ = new tsql_frag(); $$->sprintf("tsql_coalesce(%s, %s)", $3->c_str(), $5->c_str()); delete $3; delete $5; }
  | CONVERT '(' type_name ',' expr ')' { $$ = new tsql_frag(); $$->sprintf("CAST(%s AS %s)", $5->c_str(), $3); free($3); delete $5; }
  | CONVERT '(' type_name ',' expr ',' vINTEGER ')' { $$ = new tsql_frag(); $$->sprintf("convert('%s', %s, %s)", $3, $5->c_str(), $7); free($3); delete $5; free($7); }
  | DATEPART '(' vNAME ',' expr ')' { $$ = new tsql_frag(); $$->sprintf("datepart('%s'::tsql_varchar, %s)", $3, $5->c_str()); free($3); delete($5); }
  | NEWID '(' ')' { $$ = new tsql_frag(); $$->sprintf("uuid_generate_v1()"); }
  | case { $$ = $1; }
  | vVAR {
      $$ = new tsql_frag();
      make_lower($1);
      string s = $1;
      int i;
      // TODO error if var doesn't exist
      for (i = 0; i < (int)args->stmt_vars.size(); i++) {
        if (args->stmt_vars[i] == s) {
          break;
        }
      }
      if (i == (int)args->stmt_vars.size()) {
        args->stmt_vars.push_back(s);
      }
      $$->sprintf("$%ld", i + 1);
      free($1);
    }

case:
  CASE when1_list END { $$ = new tsql_frag(); $$->sprintf("CASE %s END", $2->c_str()); delete $2; }
  | CASE when1_list ELSE expr END { $$ = new tsql_frag(); $$->sprintf("CASE %s ELSE %s END", $2->c_str(), $4->c_str()); delete $2; delete $4; }
  | CASE expr when2_list END { $$ = new tsql_frag(); $$->sprintf("CASE %s %s END", $2->c_str(), $3->c_str()); delete $2; delete $3; }
  | CASE expr when2_list ELSE expr END { $$ = new tsql_frag(); $$->sprintf("CASE %s %s ELSE %s END", $2->c_str(), $3->c_str(), $5->c_str()); delete $2; delete $3; delete $5; }

when1_list:
  when1 { $$ = $1; };
  | when1_list when1 { $$ = new tsql_frag(); $$->sprintf("%s %s", $1->c_str(), $2->c_str()); delete $1; delete $2; }

when2_list:
  when2 { $$ = $1; };
  | when2_list when2 { $$ = new tsql_frag(); $$->sprintf("%s %s", $1->c_str(), $2->c_str()); delete $1; delete $2; }

when1:
  WHEN condition THEN expr { $$ = new tsql_frag(); $$->sprintf("WHEN %s THEN %s", $2->c_str(), $4->c_str()); delete $2; delete $4; }

when2:
  WHEN expr THEN expr { $$ = new tsql_frag(); $$->sprintf("WHEN %s THEN %s", $2->c_str(), $4->c_str()); delete $2; delete $4; }

declare:
  DECLARE vVAR type {
      int type;

      make_lower($3);
pprintf(WARNING, "about to get type %s", $3);
      type = types.by_name[$3 + 5].oid;
pprintf(WARNING, "got type");
      make_lower($2);
      $$ = new tsql_stmt_empty();
      args->program->add_var($2, type);
      free($2);
      free($3);
    }

open:
  OPEN vNAME {
      $$ = new tsql_stmt_empty();
      free($2);
  }

set:
  SET vVAR '=' expr {
    // TODO error if var doesn't exist
    make_lower($2);
    $$ = new tsql_stmt_set();
    ((tsql_stmt_set *)$$)->sprintf("SELECT %s", $4->c_str());
    ((tsql_stmt_set *)$$)->set_var($2);
    free($2);
    delete $4;
  }
  //| SET vNAME '=' TRANSACTION { $$ = new tsql_stmt_setopt(); ((tsql_stmt_setopt *)$$)->set_option($2); ((tsql_stmt_setopt *)$$)->set_value = $4; }
  | SET vNAME ON { $$ = new tsql_stmt_setopt(); ((tsql_stmt_setopt *)$$)->set_option($2); ((tsql_stmt_setopt *)$$)->set_value("ON"); free($2); }
  | SET vNAME OFF { $$ = new tsql_stmt_setopt(); ((tsql_stmt_setopt *)$$)->set_option($2); ((tsql_stmt_setopt *)$$)->set_value("OFF"); free($2); }
  | SET vNAME vNAME { $$ = new tsql_stmt_setopt(); ((tsql_stmt_setopt *)$$)->set_option($2); ((tsql_stmt_setopt *)$$)->set_value($3); }
  | SET vNAME vINTEGER { $$ = new tsql_stmt_setopt(); ((tsql_stmt_setopt *)$$)->set_option($2); ((tsql_stmt_setopt *)$$)->set_value($3); }
  | SET vNAME '-' vINTEGER {
      char *value;
      $$ = new tsql_stmt_setopt();
      ((tsql_stmt_setopt *)$$)->set_option($2);
      asprintf(&value, "-%s", $4);
      ((tsql_stmt_setopt *)$$)->set_value(value);
      free($2);
      free($4);
    }
  | SET TRANSACTION ISOLATION LEVEL READ COMMITTED { $$ = new tsql_stmt_empty(); }

union:
  select_frag { $$ = $1; }
  | union UNION select_frag { $$ = new tsql_frag(); $$->sprintf("%s UNION %s", $1->c_str(), $3->c_str()); delete $1; delete $3; }
  | union UNION ALL select_frag { $$ = new tsql_frag(); $$->sprintf("%s UNION ALL %s", $1->c_str(), $4->c_str()); delete $1; delete $4; }

column:
  '*' { $$ = new tsql_frag(); $$->frag = "*"; }
  | expr { $$ = $1; }
  | expr AS identifier { $$ = new tsql_frag(); $$->sprintf("%s AS \"%s\"", $1->c_str(), $3); delete $1; free($3); }
  | expr AS vLITERAL { $$ = new tsql_frag(); $$->sprintf("%s AS \"%s\"", $1->c_str(), $3); delete $1; free($3); }
  | expr AS vNLITERAL { $$ = new tsql_frag(); $$->sprintf("%s AS \"%s\"", $1->c_str(), $3); delete $1; free($3); }
  | expr identifier { $$ = new tsql_frag(); $$->sprintf("%s \"%s\"", $1->c_str(), $2); delete $1; free($2); }

columns:
  column { $$ = $1; }
  | columns ',' column { $$ = new tsql_frag(); $$->sprintf("%s, %s", $1->c_str(), $3->c_str()); delete $1; delete $3; }

table_name:
  identifier { asprintf(&$$, "\"%s\"", $1); free($1); } /* search path takes care of looking for the table */
  | identifier '.' identifier { asprintf(&$$, "\"##DATABASE##.%s\".\"%s\"", $1, $3); free($1); free($3); }
  | identifier '.' '.' identifier { asprintf(&$$, "\"%s.dbo\".\"%s\"", $1, $4); free($1); free($4); }
  | identifier '.' identifier '.' identifier { asprintf(&$$, "\"%s.%s\".\"%s\"", $1, $3, $5); free($1); free($3); free($5); }

proc_name:
  identifier { asprintf(&$$, "\"%s\"", $1); free($1); } /* search path takes care of looking for the table */
  | identifier '.' identifier {
    if (strncmp($3, "sp_", 3) == 0) {
      asprintf(&$$, "\"%s\"", $3);
    } else {
      asprintf(&$$, "\"##DATABASE##.%s\".\"%s\"", $1, $3);
    }
    free($1);
    free($3); }
  | identifier '.' identifier '.' identifier {
    if (strncmp($5, "sp_", 3) == 0) {
      asprintf(&$$, "\"%s\"", $5);
    } else {
      asprintf(&$$, "\"%s.%s\".\"%s\"", $1, $3, $5);
    }
    free($1);
    free($3);
    free($5); }
  | identifier '.' '.' identifier {
    if (strncmp($4, "sp_", 3) == 0) {
      asprintf(&$$, "\"%s\"", $4);
    } else {
      asprintf(&$$, "\"%s.dbo\".\"%s\"", $1, $4);
    }
    free($1);
    free($4); }

field_name:
  identifier { asprintf(&$$, "\"%s\"", $1); } /* search path takes care of looking for the table */
  | identifier '.' identifier { asprintf(&$$, "\"%s\".\"%s\"", $1, $3); free($1); free($3); }
  | identifier '.' identifier '.' identifier { asprintf(&$$, "\"##DATABASE##.%s\".\"%s\".\"%s\"", $1, $3, $5); free($1); free($3); free($5); }
  | identifier '.' identifier '.' identifier '.' identifier { asprintf(&$$, "\"%s.%s\".\"%s\".\"%s\"", $1, $3, $5, $7); free($1); free($3); free($5); free($7); }

from_source:
  table_name { $$ = $1; }
  | table_name identifier { asprintf(&$$, "%s AS %s", $1, $2); free($1); free($2); }
  | table_name AS identifier { asprintf(&$$, "%s AS %s", $1, $3); free($1); free($3); }
  | table_name '(' ')' { asprintf(&$$, "%s()", $1); free($1); }
  | table_name '(' ')' identifier { asprintf(&$$, "%s() AS %s", $1, $4); free($1); free($4); }
  | table_name '(' ')' AS identifier { asprintf(&$$, "%s() AS %s", $1, $5); free($1); free($5); }
  | table_name '(' params ')' { asprintf(&$$, "%s(%s)", $1, $3->c_str()); free($1); delete $3; }
  | table_name '(' params ')' identifier { asprintf(&$$, "%s(%s) AS %s", $1, $3->c_str(), $5); free($1); delete $3; free($5); }
  | table_name '(' params ')' AS identifier { asprintf(&$$, "%s(%s) AS %s", $1, $3->c_str(), $6); free($1); delete $3; free($6); }

join_type_with_on:
  JOIN { $$ = strdup("INNER JOIN"); }
  | INNER JOIN { $$ = strdup("INNER JOIN"); }
  | LEFT JOIN { $$ = strdup("LEFT OUTER JOIN"); }
  | LEFT OUTER JOIN { $$ = strdup("LEFT OUTER JOIN"); }

join_type_without_on:
  ',' { $$ = strdup(","); }
  | CROSS JOIN { $$ = strdup("CROSS JOIN"); }

join_types:
  join_type_with_on from_source ON condition { $$ = new tsql_frag(); $$->sprintf("%s %s ON %s", $1, $2, $4->c_str()); delete $4; }
  | join_type_without_on from_source { $$ = new tsql_frag(); $$->sprintf("%s %s", $1, $2);; free($1); free($2); }

joins:
  '(' joins ')' { $$ = new tsql_frag(); $$->sprintf("(%s)", $2->c_str()); delete $2; }
  | from_source join_types { $$ = new tsql_frag(); $$->sprintf("%s %s", $1, $2->c_str()); free($1); delete $2; }
  | joins join_types { $$ = new tsql_frag(); $$->sprintf("%s %s", $1->c_str(), $2->c_str()); delete $1; delete $2; }

from_clause:
  /* empty */ { $$ = new tsql_frag(); $$->frag = ""; }
  | FROM from_source { $$ = new tsql_frag(); $$->sprintf("FROM %s", $2); free($2); }
  | FROM joins { $$ = new tsql_frag(); $$->sprintf("FROM %s", $2->c_str()); delete $2; }

condition:
  expr '=' expr { $$ = new tsql_frag(); $$->sprintf("%s = %s", $1->c_str(), $3->c_str()); delete $1; delete $3; }
  | expr '<' expr { $$ = new tsql_frag(); $$->sprintf("%s < %s", $1->c_str(), $3->c_str()); delete $1; delete $3; }
  | expr '<' '=' expr { $$ = new tsql_frag(); $$->sprintf("%s <= %s", $1->c_str(), $4->c_str()); delete $1; delete $4; }
  | expr '>' expr { $$ = new tsql_frag(); $$->sprintf("%s > %s", $1->c_str(), $3->c_str()); delete $1; delete $3; }
  | expr '>' '=' expr { $$ = new tsql_frag(); $$->sprintf("%s >= %s", $1->c_str(), $4->c_str()); delete $1; delete $4; }
  | expr '<' '>' expr { $$ = new tsql_frag(); $$->sprintf("%s <> %s", $1->c_str(), $4->c_str());delete $1; delete $4; }
  | expr IN '(' union ')' { $$ = new tsql_frag(); $$->sprintf("%s IN (%s)", $1->c_str(), $4->c_str()); delete $1; delete $4; }
  | expr IN '(' params ')' { $$ = new tsql_frag(); $$->sprintf("%s IN (%s)", $1->c_str(), $4->c_str()); delete $1; delete $4; }
  | expr LIKE expr { $$ = new tsql_frag(); $$->sprintf("%s LIKE %s", $1->c_str(), $3->c_str()); delete $1; delete $3; }
  | EXISTS '(' union ')' { $$ = new tsql_frag(); $$->sprintf("EXISTS (%s)", $3->c_str()); delete $3; }
  | expr IS _NULL { $$ = new tsql_frag(); $$->sprintf("%s IS NULL", $1->c_str()); delete $1; }
  | expr IS NOT _NULL { $$ = new tsql_frag(); $$->sprintf("%s IS NOT NULL", $1->c_str()); delete $1; }
  | condition AND condition { $$ = new tsql_frag(); $$->sprintf("%s AND %s", $1->c_str(), $3->c_str()); delete $1; delete $3; }
  | condition OR condition { $$ = new tsql_frag(); $$->sprintf("%s OR %s", $1->c_str(), $3->c_str()); delete $1; delete $3; }
  | NOT condition { $$ = new tsql_frag(); $$->sprintf("NOT %s", $2->c_str()); delete $2; }
  | '(' condition ')' { $$ = new tsql_frag(); $$->sprintf("(%s)", $2->c_str()); delete $2; }

  
where_clause:
  /* empty */ { $$ = new tsql_frag(); $$->frag = ""; }
  | WHERE condition { $$ = new tsql_frag(); $$->sprintf("WHERE %s", $2->c_str()); delete $2; }

field_list:
  field_name { $$ = $1; }
  | field_list ',' field_name { asprintf(&$$, "%s, %s", $1, $3); free($1); free($3); }
  
group_by_clause:
  /* empty */ { $$ = new tsql_frag(); $$->frag = ""; }
  | GROUP BY field_list { $$ = new tsql_frag(); $$->sprintf("GROUP BY %s", $3); free($3); }
  | GROUP BY field_list HAVING condition { $$ = new tsql_frag(); $$->sprintf("GROUP BY %s HAVING %s", $3, $5->c_str()); free($3); delete $5; }

order_by_clause:
  /* empty */ { $$ = new tsql_frag(); $$->frag = ""; }
  | ORDER BY order_by_field_list { $$ = new tsql_frag(); $$->sprintf("ORDER BY %s", $3); free($3); }

order_by_field_list:
  order_by_field { $$ = $1; }
  | order_by_field_list ',' order_by_field { asprintf(&$$, "%s, %s", $1, $3); free($1); free($3); }
  
order_by_field:
  field_list { $$ = $1; }
  | field_name ASC { asprintf(&$$, "%s ASC", $1); free($1); }
  | field_name DESC { asprintf(&$$, "%s ASC", $1); free($1); }
  
assign_column:
  vVAR '=' expr {
    make_lower($1);
    $$ = new tsql_assign_column();
    $$->vars.push_back($1);
    $$->expr = strdup($3->c_str());
    free($1);
    delete $3; }

assign_columns:
  assign_column { $$ = $1; }
  | assign_columns ',' assign_column {
    $$ = $1;
    char *tmp;
    asprintf(&tmp, "%s, %s", $1->expr, $3->expr);
    $$->vars.push_back($3->vars[0]);
    free($$->expr);
    $$->expr = tmp;
    delete $1;
    delete $3; }

assign_select:
  SELECT assign_columns from_clause where_clause group_by_clause order_by_clause {
    $$ = new tsql_stmt_assign_select();
    $$->sprintf("SELECT %s %s %s %s %s", $2->expr, $3->c_str(), $4->c_str(), $5->c_str(), $6->c_str());
    $$->assign_vars = $2->vars;
    delete $2;
    delete $3;
    delete $4;
    delete $5;
    delete $6; }

//top_clause:
//  /* empty */ { $$ = new tsql_frag(); }
//  | TOP vINTEGER { $$ = new tsql_frag(); $$->sprintf("LIMIT %s", $2); free($2); }

top_clause:
  TOP vINTEGER { $$ = new tsql_frag(); $$->sprintf("LIMIT %s", $2); free($2); }

fields_clause:
  columns { $$ = new tsql_frag(); $$->sprintf("%s", $1->c_str()); delete $1; }
  | DISTINCT columns { $$ = new tsql_frag(); $$->sprintf("DISTINCT %s", $2->c_str()); delete $2; }

select_frag:
  SELECT top_clause fields_clause from_clause where_clause group_by_clause order_by_clause {
      $$ = new tsql_frag();
      $$->sprintf("SELECT %s %s %s %s %s %s", $3->c_str(), $4->c_str(), $5->c_str(), $6->c_str(), $7->c_str(), $2->c_str());
      delete $2;
      delete $3;
      delete $4;
      delete $5;
      delete $6;
      delete $7;
  }
  | SELECT fields_clause from_clause where_clause group_by_clause order_by_clause {
      $$ = new tsql_frag();
      $$->sprintf("SELECT %s %s %s %s %s", $2->c_str(), $3->c_str(), $4->c_str(), $5->c_str(), $6->c_str());
      delete $2;
      delete $3;
      delete $4;
      delete $5;
      delete $6;
  }

identifier:
  vNAME { $$ = $1; make_lower($$); }
  | vIDENTIFIER { $$ = $1; make_lower($$); }
  | vTEMPTABLE { $$ = $1; make_lower($$); }

insert:
  INSERT INTO table_name '(' field_name_list ')' VALUES '(' params ')' {
    $$ = new tsql_frag();
    $$->sprintf("INSERT INTO %s (%s) VALUES (%s);", $3, $5, $9->c_str());
    free($3);
    free($5);
    delete $9; }
  | INSERT INTO table_name VALUES '(' params ')' {
    $$ = new tsql_frag();
    $$->sprintf("INSERT INTO %s VALUES (%s);", $3, $6->c_str());
    free($3);
    delete $6; }

use:
  USE identifier {
    $$ = new tsql_stmt_use();
    $$->database = strdup($2);
    free($2);
  }

create_database:
  CREATE DATABASE identifier create_database_options {
    $$ = new tsql_stmt_create_database();
    $$->database = strdup($3);
    free($3);
  }

create_database_options:
  /* empty */
  | COLLATE vNAME { /* TODO */ }

drop_database:
  DROP DATABASE identifier {
    $$ = new tsql_stmt_drop_database();
    $$->database = strdup($3);
    free($3);
#if 0
    PGresult *res1;
    PGresult *res2;
    char *query;
    int i;

    $$->sprintf(&query, "SELECT nspname FROM pg_namespace WHERE nspname LIKE '%s%%'", $3);
    res1 = PQexec(args->conn, query);
    if (handle_error(args, res1)) {
      printf("%s\n\nquery", PQresultErrorMessage(res1));
      free(query);
    } else {
      free(query);
      for (i = 0; i < PQntuples(res1); i++) {
        $$->sprintf(&query, "DROP SCHEMA \"%s\" CASCADE", PQgetvalue(res1, i, 0));
        res2 = PQexec(args->conn, query);
        if (handle_error(args, res2)) {
          printf("%s\n\n%s", PQresultErrorMessage(res2), query);
        }
        PQclear(res2);
        free(query);
      }
    }
    PQclear(res1);
    free($3);
#endif
  }

type_name:
  vNAME { asprintf(&$$, "tsql_%s", $1); free($1); }
  | vNAME '(' vINTEGER ')' { asprintf(&$$, "tsql_%s(%s)", $1, $3); free($1); free($3); }
  | vNAME '(' vINTEGER ',' vINTEGER ')' { asprintf(&$$, "tsql_%s(%s, %s)", $1, $3, $5); free($1); free($3); free($5); }
  | NUMERIC { $$ = strdup("decimal"); }
  | NUMERIC '(' vINTEGER ')' { asprintf(&$$, "tsql_decimal(%s)", $3); free($3); }
  | NUMERIC '(' vINTEGER ',' vINTEGER ')' { asprintf(&$$, "tsql_decimal(%s, %s)", $3, $5); free($3); free($5); }
  | REAL { $$ = strdup("tsql_real"); }
  | FLOAT { $$ = strdup("tsql_float"); }
  | FLOAT '(' vINTEGER ')' {
      if (atoi($3) <= 24) {
        $$ = strdup("tsql_float");
      } else {
        $$ = strdup("tsql_real");
      }
      free($3);
    }
  | CHAR { $$ = strdup("tsql_varchar"); }
  | VARCHAR { $$ = strdup("tsql_varchar"); }
  | CHAR '(' vINTEGER ')' { asprintf(&$$, "tsql_varchar(%s)", $3); free($3); }
  | VARCHAR '(' vINTEGER ')' { asprintf(&$$, "tsql_varchar(%s)", $3); free($3); }
  | VARBINARY '(' vINTEGER ')' { asprintf(&$$, "tsql_varbinary(%s)", $3); free($3); }
  | VARCHAR '(' vNAME ')' {
      if (strcasecmp($3, "max") == 0) {
        asprintf(&$$, "tsql_varchar(max)");
      } else {
        free($3);
        YYERROR;
      }
      free($3);
    }
  | VARBINARY '(' vNAME ')' {
      if (strcasecmp($3, "max") == 0) {
        asprintf(&$$, "tsql_varbinary(max)");
      } else {
        free($3);
        YYERROR;
      }
      free($3);
    }
  | TEXT { $$ = strdup("tsql_varchar(max)"); }
  | NTEXT { $$ = strdup("tsql_nvarchar(max)"); }
  | IMAGE { $$ = strdup("tsql_varbinary(max)"); }
  | BIT { $$ = strdup("tsql_bit"); }
  | UNIQUEIDENTIFIER { $$ = strdup("uuid"); }

type:
  type_name type_modifiers { asprintf(&$$, "%s %s", $1, $2); free($1); free($2); }
  | TABLE '(' column_defs ')' { asprintf(&$$, "TABLE (%s)", $3.name_type_defs); free($3.name_type_defs); free($3.full_defs); }

type_modifiers:
  /* empty */ { $$ = strdup(""); }
  | type_modifiers type_modifier { asprintf(&$$, "%s %s", $1, $2); free($1); free($2); }
  
type_modifier:
  null_spec { $$ = $1; }
  | default_spec { $$ = $1; }

param_def:
  vVAR type { make_lower($1); $$.param_list = new vector<string>; $$.param_list->push_back($1); asprintf(&$$.type_defs, "%s", $2); asprintf(&$$.name_type_defs, "%s %s", $1, $2); free($1); free($2); }
  | vVAR type '=' expr { make_lower($1); $$.param_list = new vector<string>; $$.param_list->push_back($1); asprintf(&$$.type_defs, "%s", $2);asprintf(&$$.name_type_defs, "%s %s = %s", $1, $2, $4->c_str()); free($1); free($2); delete $4; }
  | vVAR type OUTPUT { make_lower($1); $$.param_list = new vector<string>; $$.param_list->push_back($1); asprintf(&$$.type_defs, "%s", $2); asprintf(&$$.name_type_defs, "%s %s OUTPUT", $1, $2); free($1); free($2); }
  | vVAR AS type { make_lower($1); $$.param_list = new vector<string>; $$.param_list->push_back($1); asprintf(&$$.type_defs, "%s", $3); asprintf(&$$.name_type_defs, "%s %s", $1, $3); free($1); free($3); }
  | vVAR AS type '=' expr { make_lower($1); $$.param_list = new vector<string>; $$.param_list->push_back($1); asprintf(&$$.type_defs, "%s", $3); asprintf(&$$.name_type_defs, "%s %s = %s", $1, $3, $5->c_str()); free($1); free($3); }
  | vVAR AS type OUTPUT { make_lower($1); $$.param_list = new vector<string>; $$.param_list->push_back($1); asprintf(&$$.type_defs, "%s", $3); asprintf(&$$.name_type_defs, "%s %s OUTPUT", $1, $3); free($1); free($3); }

param_defs:
  /* empty */ { $$.type_defs = strdup(""); $$.name_type_defs = strdup(""); }
  | param_def { $$ = $1; }
  | param_defs ',' param_def {
      asprintf(&$$.type_defs, "%s, %s", $1.type_defs, $3.type_defs);
      asprintf(&$$.name_type_defs, "%s, %s", $1.name_type_defs, $3.name_type_defs);
      free($1.type_defs);
      free($1.name_type_defs);
      free($3.type_defs);
      free($3.name_type_defs);
    }

function_text:
  vFUNCTION { $$ = $1; }
  /* could be done a lot better for less abuse of memory */
  | function_text vFUNCTION { asprintf(&$$, "%s\n%s", $1, $2); free($1); free($2); }

create_procedure:
  CREATE PROCEDURE table_name param_defs AS { set_scan_state(args, functiontext); } function_text {
      $$ = new tsql_frag();
      $$->sprintf("CREATE FUNCTION %s (%s) RETURNS tsql_int AS\n$TSQL$\n%s\n$TSQL$ LANGUAGE tsqlproc;", $3, $4.type_defs, $7);
      free($3);
      free($4.type_defs);
      free($4.name_type_defs);
      free($7);
    }
  | CREATE PROCEDURE table_name '(' param_defs ')' AS { set_scan_state(args, functiontext); } function_text {
      $$ = new tsql_frag();
      $$->sprintf("CREATE FUNCTION %s (%s) RETURNS tsql_int AS\n$TSQL$\n%s\n$TSQL$ LANGUAGE tsqlproc;", $3, $5.type_defs, $9);
      free($3);
      free($5.type_defs);
      free($5.name_type_defs);
      free($9);
    }

create_function:
  CREATE FUNCTION table_name '('  param_defs ')' RETURNS type AS { set_scan_state(args, functiontext); } function_text {
      $$ = new tsql_frag();
      $$->sprintf("CREATE FUNCTION %s (%s) RETURNS %s AS $TSQL$\n$TSQL$ LANGUAGE tsqlfunc;", $3, $5.type_defs, $8, $11);
      free($3);
  }
  | CREATE FUNCTION table_name '('  param_defs ')' RETURNS vVAR type AS { set_scan_state(args, functiontext); } function_text {
      make_lower($8); 
      $$ = new tsql_frag();
      $$->sprintf("CREATE FUNCTION %s (%s) RETURNS %s AS $TSQL$\nRETURNS %s\n%s\n$TSQL$ LANGUAGE tsqlfunc;", $3, $5.type_defs, $9, $8, $12);
      free($3);
  }

field_name_list:
  identifier { $$ = $1; }
  | field_name_list ',' identifier { asprintf(&$$, "%s, %s", $1, $3); free($1); free($3); }

create_view:
  CREATE VIEW table_name AS union {
      $$ = new tsql_frag();
      $$->sprintf("CREATE VIEW %s AS %s", $3, $5->c_str());
      /* check for vars (vars->isempty()) and complain if any */
      free($3);
      delete $5;
  }
  | CREATE VIEW table_name '(' field_name_list ')' AS union {
      $$ = new tsql_frag();
      $$->sprintf("CREATE VIEW %s (%s) AS %s", $3, $5, $8->c_str());
      /* check for vars and complain if any */
      free($3);
      free($5);
      delete $8;
  }

identity_spec:
  IDENTITY { $$ = strdup(""); }

null_spec:
  _NULL { $$ = strdup("NULL"); }
  | NOT _NULL { $$ = strdup("NOT NULL"); }

default_spec:
//  DEFAULT '(' expr ')' {
  DEFAULT expr {
    /* should fail any subquery or variable use here */
    asprintf(&$$, "DEFAULT %s", $2->c_str());
    delete $2;
  }

key_spec:
  /* empty */ { $$ = strdup(""); }
  | PRIMARY KEY { $$ = strdup("PRIMARY KEY"); }
  | UNIQUE { $$ = strdup("UNIQUE"); }

column_def:
  identifier type_name column_modifiers key_spec {
    asprintf(&$$.name_type_defs, "\"%s\" %s", $1, $2);
    asprintf(&$$.full_defs, "\"%s\" %s %s %s", $1, $2, $3, $4);
  }

column_defs:
  column_def { $$ = $1; }
  | column_defs ',' column_def {
     asprintf(&$$.name_type_defs, "%s, %s", $1.name_type_defs, $3.name_type_defs);
     asprintf(&$$.full_defs, "%s, %s", $1.full_defs, $3.full_defs);
     free($1.name_type_defs);
     free($1.full_defs);
     free($3.name_type_defs);
     free($3.full_defs);
  }

column_modifiers:
  /* empty */ { $$ = strdup(""); }
  | column_modifiers column_modifier { asprintf(&$$, "%s %s", $1, $2); free($1); free($2); }
  
column_modifier:
  null_spec { $$ = $1; }
  | identity_spec { $$ = strdup(""); } /* TODO: this doesn't work */
  | default_spec { $$ = $1; }

create_table:
  CREATE TABLE table_name '(' column_defs ')' {
    $$ = new tsql_frag();
    if ($3[0] == '#') {
      $$->sprintf("CREATE TEMPORARY TABLE %s (%s) WITHOUT OIDS", $3, $5.full_defs);
    } else {
      // TODO use tablespace of database
      $$->sprintf("CREATE TABLE %s (%s) WITHOUT OIDS", $3, $5.full_defs);
    }
    free($3);
    free($5.name_type_defs);
    free($5.full_defs);
  }

drop_table:
  DROP TABLE table_name { 
    $$ = new tsql_frag();
    $$->sprintf("DROP TABLE %s", $3);
    delete $3; }

constraint:
  identifier PRIMARY KEY '(' field_name_list ')' { asprintf(&$$, "\"%s\" PRIMARY KEY (%s)", $1, $5); free($1); free($5); }
  | identifier FOREIGN KEY '(' field_name_list ')' REFERENCES table_name '(' field_name_list ')' {
      asprintf(&$$, "\"%s\" FOREIGN KEY (%s) REFERENCES %s (%s)", $1, $5, $8, $10);
      free($1);
      free($5);
      free($8);
      free($10);
    }

alter_database:
  ALTER DATABASE identifier SET SINGLE_USER alter_database_single_user_with_clause { $$ = new tsql_stmt_empty(); }
  | ALTER DATABASE identifier SET RECURSIVE_TRIGGERS on_off { $$ = new tsql_stmt_empty(); }

on_off:
  ON { $$ = 0; }
  | OFF { $$ = 1; }

alter_database_single_user_with_clause:
  /* empty */ { }
  | WITH ROLLBACK IMMEDIATE { }

alter_table:
  ALTER TABLE table_name ADD CONSTRAINT constraint { $$ = new tsql_frag(); $$->sprintf("ALTER TABLE %s ADD CONSTRAINT %s", $3, $6); free($3); free($6); }
  | ALTER TABLE table_name ADD PRIMARY KEY '(' field_name_list ')' { $$ = new tsql_frag(); $$->sprintf("ALTER TABLE %s ADD PRIMARY KEY (%s)", $3, $8); free($3); free($8); }

index_modifier:
  CLUSTERED { $$ = strdup(""); }
  | NONCLUSTERED { $$ = strdup(""); }
  | UNIQUE { $$ = strdup("UNIQUE"); }

index_modifiers:
  /* empty */ { $$ = strdup(""); }
  | index_modifiers index_modifier { asprintf(&$$, "%s %s", $1, $2); free($1); free($2); }

create_index:
  CREATE index_modifiers INDEX identifier ON table_name '(' field_name_list ')' {
      $$ = new tsql_frag();
      // TODO - index names can only be 63 chars long. Need a unique id and a lookup
      $$->sprintf("CREATE %s INDEX ON %s (%s)", $2, $6, $8);
      free($2);
      free($4);
      free($6);
      free($8);
    }

