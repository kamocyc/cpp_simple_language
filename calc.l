%option noyywrap
%{
#include "syntax.h"
#include "y.tab.h"
%}

%%
[0-9]+ { yylval.dval = atoi(yytext); return NUMBER; }
[\t ] ;
"loop" { return LOOP; }
"if" { return IF; }
"break" { return BREAK; }
"append" { return APPEND; }
"get" { return GET; }
"print" { return PRINT; }
"delete" { return DELETE; }
"=" { return ASSIGN; }
"==" { return EQ; }
"!=" { return NEQ; }
[a-z] { yylval.vblno = yytext[0]-'a'; return NAME; }
\n+ { return NEWLINE; }
. { return yytext[0]; }
%%
