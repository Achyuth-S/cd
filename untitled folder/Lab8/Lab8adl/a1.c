#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 5000

char input[MAX_INPUT];
int pos = 0;
char token[100];


void next_token() {
    int i = 0;
    while (isspace((unsigned char)input[pos])) pos++;

    if (isalpha((unsigned char)input[pos])) {
        while (isalnum((unsigned char)input[pos]))
            token[i++] = input[pos++];
    }
    else if (isdigit((unsigned char)input[pos])) {
        while (isdigit((unsigned char)input[pos]))
            token[i++] = input[pos++];
    }
    else if (strchr("=<>!", input[pos])) {
        token[i++] = input[pos++];
        if (input[pos] == '=')  // ==, !=, <=, >=
            token[i++] = input[pos++];
    }
    else if (strchr("()+-*/;{}[],", input[pos])) {
        token[i++] = input[pos++];
    }
    else if (input[pos] == '\0') {
    }
    else {
        fprintf(stderr, "Invalid character '%c'\n", input[pos]);
        exit(1);
    }

    token[i] = '\0';
}

void error(const char *expect) {
    fprintf(stderr, "Syntax error: expected '%s', found '%s'\n",
            expect, token);
    exit(1);
}

void match(const char *expect) {
    if (strcmp(token, expect) == 0)
        next_token();
    else
        error(expect);
}

void parse_program(), declarations(), identifier_list();
void statement_list(), statement(), assign_stat();
void expn(), eprime(), simple_expn(), seprime();
void term(), tprime(), factor();



void parse_program() {
    match("main"); match("("); match(")"); match("{");
    declarations();
    statement_list();
    match("}");
}

void declarations() {
    while (strcmp(token, "int")==0
        || strcmp(token, "float")==0
        || strcmp(token, "char")==0) {
        next_token();
        identifier_list();
        match(";");
    }
}

void identifier_list() {
    if (isalpha((unsigned char)token[0])) {
        next_token();
    } else error("identifier");

    if (strcmp(token, "[")==0) {
        match("[");
        if (isdigit((unsigned char)token[0]))
            next_token();
        else error("number");
        match("]");
    }

    while (strcmp(token, ",")==0) {
        match(",");
        if (isalpha((unsigned char)token[0])) {
            next_token();
        } else error("identifier");
        if (strcmp(token, "[")==0) {
            match("[");
            if (isdigit((unsigned char)token[0]))
                next_token();
            else error("number");
            match("]");
        }
    }
}

void statement_list() {
    while (isalpha((unsigned char)token[0])) {
        statement();
    }
}


void statement() {
    assign_stat();
    match(";");
}


void assign_stat() {
    if (isalpha((unsigned char)token[0])) {
        next_token();
        
        if (strcmp(token, "[") == 0) {
            match("[");
            expn();
            match("]");
        }
    } else error("identifier");

    match("=");  
    expn();    
}

void expn() {
    simple_expn();
    eprime();
}

void eprime() {
    if (strcmp(token, "==")==0 || strcmp(token, "!=")==0 ||
        strcmp(token, "<=")==0 || strcmp(token, ">=")==0 ||
        strcmp(token, "<")==0  || strcmp(token, ">")==0) {
        next_token();
        simple_expn();
    }
}


void simple_expn() {
    term();
    seprime();
}


void seprime() {
    while (strcmp(token, "+")==0 || strcmp(token, "-")==0) {
        next_token();
        term();
    }
}


void term() {
    factor();
    tprime();
}

void tprime() {
    while (strcmp(token, "*")==0 || strcmp(token, "/")==0) {
        next_token();
        factor();
    }
}


void factor() {
    if (strcmp(token, "(") == 0) {
        match("(");
        expn();
        match(")");
    }
    else if (strcmp(token, "+")==0 || strcmp(token,"-")==0) {
        next_token();
        factor();
    }
    else if (isalpha((unsigned char)token[0])) {   
        next_token();
        if (strcmp(token, "[") == 0) {  
            match("[");
            expn();
            match("]");
        }
    }
    else if (isdigit((unsigned char)token[0])) { 
        next_token();
    }
    else {
        error("identifier, number, or '('");
    }
}

int main() {
    size_t used = 0;
    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (used + len < MAX_INPUT-1) {
            memcpy(input + used, line, len);
            used += len;
        }
    }
    input[used] = '\0';

    pos = 0;
    next_token();
    parse_program();

    if (token[0] == '\0') {
        printf("Parsing successful!\n");
        return 0;
    } else {
        fprintf(stderr, "Parsing incomplete: leftover '%s'\n", token);
        return 1;
    }
}

/*
main()
{
    int a, b, c[5];
    a = - ( b + c[2] ) * ( a - 3 );
    b = (a + 5) / - ( b - 1 );
    c[0] = ( a + b ) * ( c[1] - 2 );
}
Parsing successful!
*/
