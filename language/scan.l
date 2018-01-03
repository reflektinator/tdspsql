%option noyywrap
%option nounput
%option noinput
%option reentrant
%option bison-bridge
%option bison-locations
%option stack
%option outfile="scan.l.cc" header-file="scan.l.h"
%option yylineno

%{
#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng-1;
%}

%{
#include <stdint.h>
#include "language.h"
#include "parse.y.h"

%}

%x literal
%x nliteral
%x set
%x comment
%x functiontext
%x with

%%

(?i:ADD)            { return ADD; }
(?i:ALL)            { return ALL; }
(?i:ALTER)          { return ALTER; }
(?i:AND)            { return AND; }
(?i:AS)             { return AS; }
(?i:ASC)                              { return ASC; }
(?i:BY)             { return BY; }
(?i:BEGIN)          { return _BEGIN; }
(?i:BIT)            { return BIT; }
(?i:CASE)           { return CASE; }
(?i:CAST)           { return CAST; }
(?i:CHAR)           { return CHAR; }
(?i:COALESCE)       { return COALESCE; }
(?i:COLLATE)        { return COLLATE; }
(?i:COMMITTED)      { return COMMITTED; }
(?i:CONVERT)        { return CONVERT; }
(?i:CONSTRAINT)     { return CONSTRAINT; }
(?i:COUNT)          { return COUNT; }
(?i:CLUSTERED)      { return CLUSTERED; }
(?i:CREATE)         { return CREATE; }
(?i:CROSS)          { return CROSS; }
(?i:CURSOR)         { return CURSOR; }
(?i:DATABASE)       { return DATABASE; }
(?i:DATEPART)       { return DATEPART; }
(?i:DEALLOCATE)                       { return DEALLOCATE; }
(?i:DECLARE)                          { return DECLARE; }
(?i:DEFAULT)                          { return DEFAULT; }
(?i:DELETE)                           { return DELETE; }
(?i:DESC)                             { return DESC; }
(?i:DISTINCT)                         { return DISTINCT; }
(?i:DROP)                             { return DROP; }
(?i:ELSE)           { return ELSE; }
(?i:END)            { return END; }
(?i:EXISTS)         { return EXISTS; }
(?i:EXEC)           { return EXEC; }
(?i:EXECUTE)        { return EXECUTE; }
(?i:FETCH)          { return FETCH; }
(?i:FLOAT)          { return FLOAT; }
(?i:FOR)            { return FOR; }
(?i:FOREIGN)        { return FOREIGN; }
(?i:FROM)           { return FROM; }
(?i:FUNCTION)                         { return FUNCTION; }
(?i:GRANT)                            { return GRANT; }
(?i:GROUP)                            { return GROUP; }
(?i:HAVING)                           { return HAVING; }
(?i:IDENTITY)                         { return IDENTITY; }
(?i:IF)                               { return IF; }
(?i:IMAGE)                            { return IMAGE; }
(?i:IMMEDIATE)                        { return IMMEDIATE; }
(?i:IN)                               { return IN; }
(?i:INDEX)                            { return INDEX; }
(?i:INNER)                            { return INNER; }
(?i:INSERT)                           { return INSERT; }
(?i:INTEGER)                          { yylval->strval = strdup("int"); return vNAME; }
(?i:INTO)                             { return INTO; }
(?i:IS)                               { return IS; }
(?i:ISNULL)                           { return COALESCE; }
(?i:ISOLATION)                        { return ISOLATION; }
(?i:JOIN)           { return JOIN; }
(?i:KEY)            { return KEY; }
(?i:LEFT)           { return LEFT; }
(?i:LEVEL)          { return LEVEL; }
(?i:LIKE)           { return LIKE; }
(?i:LOGIN)          { return LOGIN; }
(?i:NEWID)          { return NEWID; }
(?i:NEXT)           { return NEXT; }
(?i:NONCLUSTERED)                     { return NONCLUSTERED; }
(?i:NOT)                              { return NOT; }
(?i:NTEXT)                            { return NTEXT; }
(?i:NULL)                             { return _NULL; }
(?i:NUMERIC)                          { return NUMERIC; }
(?i:OUTPUT)                           { return OUTPUT; }
(?i:OUTER)                            { return OUTER; }
(?i:ORDER)                            { return ORDER; }
(?i:OFF)                              { return OFF; }
(?i:ON)                               { return ON; }
(?i:OPEN)                             { return OPEN; }
(?i:OR)                               { return OR; }
<with>(?i:PASSWORD)                   { return PASSWORD; }
(?i:PRIMARY)                          { return PRIMARY; }
(?i:PRINT)                            { return PRINT; }
(?i:PROC)                             { return PROCEDURE; }
(?i:PROCEDURE)                        { return PROCEDURE; }
(?i:REAL)                             { return REAL; }
(?i:READ)                             { return READ; }
<set,INITIAL>(?i:RECURSIVE_TRIGGERS)  { BEGIN(INITIAL); return RECURSIVE_TRIGGERS; }
(?i:REFERENCES)                       { return REFERENCES; }
(?i:RETURNS)                          { return RETURNS; }
(?i:ROLLBACK)                         { return ROLLBACK; }
(?i:SELECT)                           { return SELECT; }
<set,INITIAL>(?i:SINGLE_USER)         { BEGIN(INITIAL); return SINGLE_USER; }
(?i:SERVER)                           { return SERVER; }
(?i:SET)                              { BEGIN(set); return SET; }
(?i:TABLE)                            { return TABLE; }
(?i:TEXT)                             { return TEXT; }
(?i:THEN)                             { return THEN; }
<set,INITIAL>(?i:TRANSACTION)         { BEGIN(INITIAL); return TRANSACTION; }
(?i:TO)                               { return TO; }
(?i:TOP)                              { return TOP; }
(?i:USE)                              { return USE; }
(?i:USER)                             { return USER; }
(?i:UNION)                            { return UNION; }
(?i:UNIQUE)                           { return UNIQUE; }
(?i:UNIQUEIDENTIFIER)                 { return UNIQUEIDENTIFIER; }
(?i:UPDATE)                           { return UPDATE; }
(?i:VALUES)                           { return VALUES; }
(?i:VIEW)                             { return VIEW; }
(?i:VARCHAR)                          { return VARCHAR; }
(?i:VARBINARY)                        { return VARBINARY; }
(?i:WHEN)                             { return WHEN; }
(?i:WHERE)                            { return WHERE; }
(?i:WHILE)                            { return WHILE; }
(?i:WITH)                             { return WITH; }

[=()[\],\.*+-/<>&]                     { return yytext[0]; }

'                                     { yy_push_state(literal, yyscanner); }
N'                                    { yy_push_state(nliteral, yyscanner); }
"/*"                                  { yy_push_state(comment, yyscanner); }

0[Xx][0-9A-Fa-f]+                     { pprintf(WARNING, "rrr\n"); asprintf(&yylval->strval, "%ld", strtol(yytext, NULL, 16)); pprintf(WARNING, "sss\n"); pprintf(WARNING, "ttt %s\n", yylval->strval); return vINTEGER; }
[0-9]+                                { yylval->strval = strdup(yytext); return vINTEGER; }
[0-9]*\.[0-9]+                        { yylval->strval = strdup(yytext); return vDECIMAL; }
[0-9]+\.[0-9]*                        { yylval->strval = strdup(yytext); return vDECIMAL; }
<set>[A-Za-z][A-Za-z0-9_]*            { BEGIN(INITIAL); yylval->strval = strdup(yytext); return vNAME; }
<set>@[@A-Za-z_][@A-Za-z0-9_]*         { BEGIN(INITIAL); yylval->strval = strdup(yytext); return vVAR; }
[A-Za-z][A-Za-z0-9_]*                 { yylval->strval = strdup(yytext); return vNAME; }
@[@A-Za-z_][@A-Za-z0-9_]*              { yylval->strval = strdup(yytext); return vVAR; }
#[A-Za-z][A-Za-z0-9_]*                { yylval->strval = strdup(yytext); return vTEMPTABLE; }
\[[A-Za-z0-9_/ ]+\]                   { yylval->strval = strdup(yytext + 1); yylval->strval[yyleng - 2] = 0; return vIDENTIFIER; }
\"[A-Za-z0-9_/ ]+\"                   { yylval->strval = strdup(yytext + 1); yylval->strval[yyleng - 2] = 0; return vIDENTIFIER; }
--[^\n]*                              { }

[ ;\r\t]+                             { }
[ \n]                                 { }
.                                     { printf("unknown character %02hhx (%c)\n", yytext[0], (yytext[0] > ' ')?yytext[0]:' '); }

<functiontext>.*                      { yylval->strval = strdup(yytext); return vFUNCTION; }
<functiontext>[\r\n]+                 { }

<literal>'                            { yy_pop_state(yyscanner); yytext[yyleng - 1] = 0; yylval->strval = strdup(yytext); return vLITERAL; }
<literal>''                           { yymore(); }
<literal>[^']*                        { yymore(); }

<nliteral>'                           { yy_pop_state(yyscanner); yytext[yyleng - 1] = 0; yylval->strval = strdup(yytext); return vNLITERAL; }
<nliteral>''                          { yymore(); }
<nliteral>[^']*                       { yymore(); }

<comment>"*/"                         { yy_pop_state(yyscanner); }
<comment>.                            { }
%%

void
set_scan_state(bison_args *args, int state) {
  struct yyguts_t * yyg = (struct yyguts_t*)args->scanner;
  BEGIN(state);
}

/* this suppresses -Wunused-function compiler warning */
static int yy_top_state(yyscan_t yyscanner) __attribute__((used));

