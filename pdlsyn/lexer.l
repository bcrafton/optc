  // FLEX file example1.l
%{
#include <cstdio>
#include <cstdlib>
#include <string> // this has to come before ... 

#include "fsm.h"
#include "protocol.h"
#include "transition.h"
#include "body.h"

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

"protocol"             return PROTOCOL;
"endprotocol"          return ENDPROTOCOL;

"ports"                return PORTS;
"endports"             return ENDPORTS;

"init"                 return INIT;
"endinit"              return ENDINIT;

"behavior"             return BEHAVIOR;
"endbehavior"          return ENDBEHAVIOR;

"packet"               return PACKET;
"subpacket"            return SUBPACKET;

"="                    return ASSIGN;
"|"                    return ORBAR;
"+"                    return PLUS;
";"                    return SEMICOLON;
":"                    return COLON;
"("                    return LPAREN;
")"                    return RPAREN;
"{"                    return LBRACKET;
"}"                    return RBRACKET;
"in"                   return IN;
"out"                  return OUT;
"data"                 return DATA;
"control"              return CONTROL;
"state"                return STATE;

"generate"             return GENERATE;

"map"                  return MAP;

"@"                    return ATSIGN;
"send"                 return SEND;
"rec"                  return RECEIVE;


"begin"                return BEGIN_BLOCK;
"end"                  return END_BLOCK;
","                    return COMMA;

[a-zA-Z_\.\/][a-zA-Z0-9_\.\/]*   {yylval.str_val = new string(yytext); return VARIABLE;}

"->"                   return RARROW;



%%

void yyerror(char* str) {
  printf("Parse Error: \n%s\n",str);
}

int yywrap(void) { }

