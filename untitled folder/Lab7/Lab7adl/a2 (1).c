#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *input;
int pos = 0, row = 1, col = 1;

void error(const char *expected) {
    printf("Syntax Error: Expected '%s' at line %d, column %d\n", expected, row, col);
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

int matchKeyword(const char *kw) {
    skipWhitespace();
    int len = strlen(kw);
    if (strncmp(&input[pos], kw, len) == 0 && !isalnum(input[pos + len])) {
        pos += len;
        col += len;
        return 1;
    }
    return 0;
}

void data_type() {
    if (!matchKeyword("int") && !matchKeyword("char")) {
        error("data type (int/char)");
    }
}

void pointer_decl() {
    skipWhitespace();
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
    skipWhitespace();
    if (peek() == '\0') return;

    if (strncmp(&input[pos], "int", 3) == 0 || strncmp(&input[pos], "char", 4) == 0) {
        data_type();
        pointer_list();
        match(';');
    } else {
        error("data type");
    }
}

void parse_declarations() {
    while (peek() != '\0') {
        declaration();
        skipWhitespace();
    }
}

int main() {
    static char buffer[4096];
    printf("Enter variable declarations (multiple lines supported):\n");

    size_t total_len = 0;
    while (fgets(buffer + total_len, sizeof(buffer) - total_len, stdin)) {
        total_len = strlen(buffer);
        if (total_len > 0 && buffer[total_len - 1] == '\n') break; // Stop if line ends
    }

    input = buffer;

    parse_declarations();

    printf("Parsing successful! All declarations are valid.\n");
    return 0;
}

/*Enter variable declarations (multiple lines supported):
int *p, **q;char **a, *b;
Parsing successful! All declarations are valid. */
