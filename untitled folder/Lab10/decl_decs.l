%{
#include "decl_decs.tab.h"
%}

%%
"int"       { return DTYPE; }
"float"     { return DTYPE; }
"char"      { return DTYPE; }
"if"        { return IF; }
"else"      { return ELSE; }
"{"         { return LC; }
"}"         { return RC; }
"("         { return LP; }
")"         { return RP; }
";"         { return SEMICOLON; }
","         { return COMMA; }
"="         { return EQUAL; }
[0-9]+      { return NUM; }
[a-zA-Z_][a-zA-Z0-9_]*  { return ID; }
\n          { return NL; }
[ \t]       ; // Ignore whitespace
.           { return *yytext; }
%%

int yywrap() {
    return 1;
}
