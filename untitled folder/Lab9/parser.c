#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

char tokens[MAX][MAX];
int current = 0, total = 0;

int is_id(char *t) { return isalpha(t[0]); }

void error(const char *msg) {
    printf("Syntax Error: %s at token '%s'\n", msg, tokens[current]);
    exit(1);
}

void match(const char *expected) {
    if (strcmp(tokens[current], expected) == 0)
        printf("Matched: %s\n", tokens[current++]);
    else {
        char msg[100];
        sprintf(msg, "Expected '%s'", expected);
        error(msg);
    }
}

void expn() {
    if (is_id(tokens[current]) || isdigit(tokens[current][0])) {
        match(tokens[current]);
        if (strchr("+-*/<>", tokens[current][0]) || !strcmp(tokens[current], "==") || !strcmp(tokens[current], "!="))
            match(tokens[current]), expn();
    } else error("Invalid expression");
}

void assign(int has_semicolon) {
    if (!is_id(tokens[current])) error("Expected identifier");
    match(tokens[current]); match("="); expn();
    if (has_semicolon) match(";");
}

void statement();

void stmt_list() {
    while (strcmp(tokens[current], "}") && current < total)
        statement();
}

void decision() {
    match("if"); match("("); expn(); match(")"); match("{");
    stmt_list(); match("}"); 
    if (!strcmp(tokens[current], "else"))
        match("else"), match("{"), stmt_list(), match("}");
}

void loop() {
    if (!strcmp(tokens[current], "while"))
        match("while"), match("("), expn(), match(")"), match("{"), stmt_list(), match("}");
    else if (!strcmp(tokens[current], "for")) {
        match("for"); match("(");
        assign(0); match(";"); expn(); match(";"); assign(0);
        match(")"); match("{"); stmt_list(); match("}");
    } else error("Invalid loop statement");
}

void statement() {
    if (!strcmp(tokens[current], "if")) decision();
    else if (!strcmp(tokens[current], "while") || !strcmp(tokens[current], "for")) loop();
    else if (is_id(tokens[current])) assign(1);
    else error("Invalid start of statement");
}

void tokenize(char *input) {
    char *token = strtok(input, " \n\t");
    while (token) {
        int len = strlen(token), i = 0;
        while (i < len) {
            if (ispunct(token[i]) && token[i] != '_') {
                if (i) {
                    strncpy(tokens[total], token, i);
                    tokens[total++][i] = '\0';
                }
                char punct[3] = {token[i], '\0', '\0'};
                if ((strchr("=!<>", token[i])) && token[i + 1] == '=') {
                    punct[1] = '='; strcpy(tokens[total++], punct);
                    token += i + 2;
                } else {
                    strcpy(tokens[total++], punct);
                    token += i + 1;
                }
                len = strlen(token); i = 0;
            } else i++;
        }
        if (strlen(token)) strcpy(tokens[total++], token);
        token = strtok(NULL, " \n\t");
    }
    tokens[total][0] = '\0';
}

int main() {
    FILE *fp = fopen("input.c", "r");
    if (!fp) return printf("Error opening input.c\n"), 1;
    
    char buffer[1000]; fread(buffer, 1, sizeof(buffer), fp); fclose(fp);
    tokenize(buffer);

    while (current < total) statement();
    printf(current == total ? "Parsing Successful.\n" : "Parsing Incomplete. Remaining token: '%s'\n", tokens[current]);

    return 0;
}
