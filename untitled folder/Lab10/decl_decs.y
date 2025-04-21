%{
#include <stdio.h>
#include <stdlib.h>

void yyerror(const char *msg);
int yylex(void);
%}

%token ID NUM DTYPE SEMICOLON COMMA EQUAL
%token IF ELSE LP RP LC RC NL

%%
stmt: declaration_stmt NL { printf("Valid Declaration Statement\n"); exit(0); }
    | decision_stmt NL    { printf("Valid Decision-Making Statement\n"); exit(0); }
    ;

declaration_stmt: DTYPE decl_list SEMICOLON ;

decl_list: var
         | var COMMA decl_list ;

var: ID
   | ID EQUAL NUM ;

decision_stmt: IF LP condition RP LC inner_stmt RC ELSE LC inner_stmt RC
             | IF LP condition RP LC inner_stmt RC
             ;

condition: ID  // For simplicity, treat variables as condition
         | NUM ;

inner_stmt: assignment_stmt
          | declaration_stmt ;

assignment_stmt: ID EQUAL ID SEMICOLON
               | ID EQUAL NUM SEMICOLON ;
%%
void yyerror(const char *msg) {
    printf("Invalid Expression\n");
    exit(0);
}

int main() {
    printf("Enter a statement:\n");
    yyparse();
    return 0;
}
