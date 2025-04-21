#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *input;
int pos = 0, row = 1, col = 1;

void error(const char *expected) {
    printf("Syntax Error: Expected '%s' at row %d, column %d\n", expected, row, col);
    exit(1);
}

void advance() {
    if (input[pos] == '\n') {
        row++;
        col = 1;
    } else {
        col++;
    }
    pos++;
}

char peek() {
    return input[pos];
}

void skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

void match(char expected) {
    skipWhitespace();
    if (peek() == expected) {
        advance();
    } else {
        char exp[2] = {expected, '\0'};
        error(exp);
    }
}

void identifier() {
    skipWhitespace();
    if (isalpha(peek()) || peek() == '_') {
        while (isalnum(peek()) || peek() == '_') {
            advance();
        }
    } else {
        error("identifier");
    }
}

void data_type() {
    skipWhitespace();
    if (strncmp(&input[pos], "int", 3) == 0) {
        pos += 3;
        col += 3;
    } else if (strncmp(&input[pos], "char", 4) == 0) {
        pos += 4;
        col += 4;
    } else {
        error("data type (int/char)");
    }
}

void pointer_decl() {
    skipWhitespace();
    // Match zero or more '*'
    while (peek() == '*') {
        match('*');
    }
    identifier();
}

void pointer_list() {
    pointer_decl();
    skipWhitespace();
    while (peek() == ',') {
        match(',');
        pointer_decl();
        skipWhitespace();
    }
}

void declaration() {
    data_type();
    pointer_list();
    match(';');
}

int main() {
    static char buffer[1024];

    printf("Enter a declaration statement:\n");
    fgets(buffer, sizeof(buffer), stdin);
    input = buffer;

    declaration();

    skipWhitespace();
    if (peek() != '\0') {
        error("end of input");
    }

    printf("Parsing successful! Pointer declaration is valid.\n");
    return 0;
}

/*Enter a declaration statement:
int *p, **q;
char **a, *b;
int x, *y, **z;*/
