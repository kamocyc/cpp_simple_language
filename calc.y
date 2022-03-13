%{
#include "syntax.h"
#include <vector>
#include <iostream>
extern int yylex();
extern int yyerror(Expr**,  const char* );
extern char* yytext;
extern "C" FILE *yyin;

#define YYDEBUG 1
%}
%parse-param { Expr** root }
%union{
  int dval;
  int vblno;
  Expr* tree;
}
%token <vblno> NAME
%token <dval> NUMBER
%token LOOP IF ASSIGN EQ NEQ BREAK APPEND GET PRINT DELETE
%token NEWLINE
%type <tree> expression addexp mulexp primary statement statement_list block
%%
program : statement_list { *root = $1; }
        | NEWLINE statement_list { *root = $2; }
        | statement_list NEWLINE { *root = $1; }
        | NEWLINE statement_list NEWLINE { *root = $2; }
;

statement_list : statement { $$ = $1; }
               | statement_list NEWLINE statement { $$ = new SeqExpr {$1, $3}; }
;

statement : NAME ASSIGN expression { $$ = new AssignmentExpr {$1, $3}; }
          | LOOP block { $$ = new LoopExpr ($2); }
          | IF '(' expression ')' block { $$ = new IfExpr($3, $5); } 
          | expression { $$ = $1; }
          | BREAK { $$ = new BreakExpr; }
          | APPEND NAME expression { $$ = new AppendExpr {$2, $3}; }
          | GET NAME expression { $$ = new GetExpr {$2, $3}; }
          | DELETE NAME { $$ = new DeleteExpr {$2}; }
          | PRINT expression { $$ = new PrintExpr {$2}; }
;

block : '{' statement_list '}' { $$ = $2; }
      | '{' NEWLINE statement_list '}' { $$ = $3; };
      | '{' statement_list NEWLINE '}' { $$ = $2; };
      | '{' NEWLINE statement_list NEWLINE '}' { $$ = $3; };

expression : addexp { $$ = $1; }
           | expression '<' addexp { $$ = new CompExpr {CompOp::Lt, {$1, $3}}; }
           | expression '>' addexp { $$ = new CompExpr {CompOp::Gt, {$1, $3}}; }
           | expression EQ addexp { $$ = new CompExpr {CompOp::Eq, {$1, $3}}; }
           | expression NEQ addexp { $$ = new CompExpr {CompOp::Neq, {$1, $3}}; }
;

addexp : mulexp { $$ = $1; }
       | addexp '+' mulexp { $$ = new ArithExpr {Op::Add, {$1, $3}}; }
       | addexp '-' mulexp { $$ = new ArithExpr {Op::Sub, {$1, $3}}; }
;

mulexp : mulexp '*' primary { $$ = new ArithExpr {Op::Mul, {$1, $3}}; }
       | mulexp '/' primary { $$ = new ArithExpr {Op::Div, {$1, $3}}; }
       | primary { $$ = $1; }
;

primary : '(' expression ')' { $$ = $2; }
        | NUMBER { $$ = new IntLiteralExpr {$1}; }
        | NAME { $$ = new VariableExpr {$1}; }
;
%%
int yyerror(Expr** root, char const* str )
{
  std::cerr << "parser error near " << yytext << std::endl;
	return 0 ;
}