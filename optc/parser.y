%{
#include "defines.h"

using namespace std;
using namespace boost;

extern int yylex();
extern void yyerror(char*);
void Div0Error(void);
void UnknownVarError(string s);

%}

%union {
  int     int_val;
  double  double_val;
  string* str_val;
}

%start top
%token <int_val>    BEGIN_BLOCK END_BLOCK
%token <int_val>    FOR RPAREN LPAREN SEMICOLON COLON PLUS ORBAR COMMA ASSIGN LBRACKET RBRACKET
%token <str_val>    VARIABLE
%token <double_val> NUMBER

%%

top           : exprs 
                {
                } ; 

exprs         : exprs expr
                {
                }
              | expr 
                {
                } ; 
  
expr          : add_op
                {
                } ;

add_op        : NUMBER PLUS NUMBER
                {
                } ;
                
%%





























