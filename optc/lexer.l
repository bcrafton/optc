  // FLEX file example1.l
%{
#include "defines.h"

#include <cstdio>
#include <cstdlib>
#include <string> // this has to come before ... 

using namespace std;
using namespace boost;

#include "parser.tab.h"

void yyerror(char*);
int yyparse(void);
%}

%%
"#".*                  ;  // throw out comments
[ \t\n]+               ;  // do nothing on whitespace
[0-9][0-9]*(.[0-9]+)?  {yylval.double_val = atof(yytext); return NUMBER;}

"for"                  return FOR;
"begin"                return BEGIN_BLOCK;
"end"                  return END_BLOCK;
","                    return COMMA;
"="                    return ASSIGN;
"|"                    return ORBAR;
"+"                    return PLUS;
";"                    return SEMICOLON;
":"                    return COLON;
"("                    return LPAREN;
")"                    return RPAREN;
"{"                    return LBRACKET;
"}"                    return RBRACKET;

[a-zA-Z_\.\/][a-zA-Z0-9_\.\/]*   {yylval.str_val = new string(yytext); return VARIABLE;}

%%

void yyerror(char* str) {
  printf("Parse Error: \n%s\n",str);
}

int yywrap(void) { }

