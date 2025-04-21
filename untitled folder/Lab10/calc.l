%{
#include "calc.tab.h"  // includes token definitions from Bison
%}
    
%%
[0-9]+     { yylval = atoi(yytext); return NUM; }
"+"        { return '+'; }
"-"        { return '-'; }
"*"        { return '*'; }
"/"        { return '/'; }
"^"        { return '^'; }
"n"        { return 'n'; }
\n         { return NEWLINE; }
[ \t]      { /* skip spaces */ }
.          { return yytext[0]; }
%%
    
int yywrap() {
    return 1;
}
    