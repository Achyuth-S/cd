%{
    
#include <stdio.h>
#include <stdlib.h>
    
    // Declare yylex and yyerror so Bison knows about them
int yylex(void);
void yyerror(const char *msg);

%}
    
%token NUM NEWLINE
%left '+' '-' '*' '/' '^' 'n'
    
%%
    
input:
    input line
    | /* empty */ ;
    
line:
    NEWLINE
    | exp NEWLINE {printf("Valid Postfix Expression\n"); };
    
exp:
    NUM
    | exp exp '+' 
    | exp exp '-' 
    | exp exp '*' 
    | exp exp '/' 
    | exp exp '^' 
    | exp 'n' ;

%%
    
void yyerror(const char *msg) {
        printf("Error: %s\n", msg);
}
    
int main() {
        printf("Enter postfix expressions:\n");
        yyparse();
        return 0;
}
    