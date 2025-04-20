#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    TOKEN_MAIN,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_INT,
    TOKEN_CHAR,
    TOKEN_ID,
    TOKEN_NUM,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_EQUALS,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[256];
    int row;
    int col;
} Token;

FILE *input_file;
Token current_token;
int current_row = 1;
int current_col = 1;

void error(const char *expected, int row, int col);
void get_token();
void match(TokenType expected_type);
void program();
void declarations();
void data_type();
void identifier_list();
void assign_stat();

void error(const char *expected, int row, int col) {
    printf("Error at row %d, column %d: Expected %s\n", row, col, expected);
    exit(1);
}

int get_next_char() {
    int c = fgetc(input_file);
    if (c == '\n') {
        current_row++;
        current_col = 1;
    } else {
        current_col++;
    }
    return c;
}

void unget_char(int c) {
    ungetc(c, input_file);
    if (c == '\n') {
        current_row--;
    } else {
        current_col--;
    }}

void get_token() {
    int c;
    
    do {
        c = get_next_char();
    } while (isspace(c));
    
    current_token.row = current_row;
    current_token.col = current_col - 1;  
    
    if (c == EOF) {
        current_token.type = TOKEN_EOF;
        strcpy(current_token.lexeme, "EOF");
        return;
    }
    
    switch (c) {
        case '(':
            current_token.type = TOKEN_LPAREN;
            strcpy(current_token.lexeme, "(");
            return;
        case ')':
            current_token.type = TOKEN_RPAREN;
            strcpy(current_token.lexeme, ")");
            return;
        case '{':
            current_token.type = TOKEN_LBRACE;
            strcpy(current_token.lexeme, "{");
            return;
        case '}':
            current_token.type = TOKEN_RBRACE;
            strcpy(current_token.lexeme, "}");
            return;
        case ';':
            current_token.type = TOKEN_SEMICOLON;
            strcpy(current_token.lexeme, ";");
            return;
        case ',':
            current_token.type = TOKEN_COMMA;
            strcpy(current_token.lexeme, ",");
            return;
        case '=':
            current_token.type = TOKEN_EQUALS;
            strcpy(current_token.lexeme, "=");
            return;
    }
    
    if (isalpha(c)) {
        int i = 0;
        current_token.lexeme[i++] = c;
        
        while (isalnum(c = get_next_char())) {
            current_token.lexeme[i++] = c;
        }
        unget_char(c);  
        
        current_token.lexeme[i] = '\0';
        
        if (strcmp(current_token.lexeme, "main") == 0) {
            current_token.type = TOKEN_MAIN;
        } else if (strcmp(current_token.lexeme, "int") == 0) {
            current_token.type = TOKEN_INT;
        } else if (strcmp(current_token.lexeme, "char") == 0) {
            current_token.type = TOKEN_CHAR;
        } else {
            current_token.type = TOKEN_ID;
        }
        
        return;
    }

    if (isdigit(c)) {
        int i = 0;
        current_token.lexeme[i++] = c;
        
        while (isdigit(c = get_next_char())) {
            current_token.lexeme[i++] = c;
        }
        unget_char(c);  
        
        current_token.lexeme[i] = '\0';
        current_token.type = TOKEN_NUM;
        
        return;
    }
    
    current_token.type = TOKEN_ERROR;
    current_token.lexeme[0] = c;
    current_token.lexeme[1] = '\0';
    printf("Error at row %d, column %d: Unexpected character '%c'\n", 
           current_row, current_col - 1, c);
    exit(1);
}

void match(TokenType expected_type) {
    if (current_token.type == expected_type) {
        get_token(); 
    } else {
        char expected[50];
        switch (expected_type) {
            case TOKEN_MAIN: strcpy(expected, "'main'"); break;
            case TOKEN_LPAREN: strcpy(expected, "'('"); break;
            case TOKEN_RPAREN: strcpy(expected, "')'"); break;
            case TOKEN_LBRACE: strcpy(expected, "'{'"); break;
            case TOKEN_RBRACE: strcpy(expected, "'}'"); break;
            case TOKEN_INT: strcpy(expected, "'int'"); break;
            case TOKEN_CHAR: strcpy(expected, "'char'"); break;
            case TOKEN_ID: strcpy(expected, "identifier"); break;
            case TOKEN_NUM: strcpy(expected, "number"); break;
            case TOKEN_SEMICOLON: strcpy(expected, "';'"); break;
            case TOKEN_COMMA: strcpy(expected, "','"); break;
            case TOKEN_EQUALS: strcpy(expected, "'='"); break;
            default: strcpy(expected, "unknown token"); break;
        }
        error(expected, current_token.row, current_token.col);
    }}

void program() {
    match(TOKEN_MAIN);
    match(TOKEN_LPAREN);
    match(TOKEN_RPAREN);
    match(TOKEN_LBRACE);
    declarations();
    assign_stat();
    match(TOKEN_RBRACE);
}


void declarations() {
    if (current_token.type == TOKEN_INT || current_token.type == TOKEN_CHAR) {
        data_type();
        identifier_list();
        match(TOKEN_SEMICOLON);
        declarations();  
    }
}

void data_type() {
    if (current_token.type == TOKEN_INT) {
        match(TOKEN_INT);
    } else if (current_token.type == TOKEN_CHAR) {
        match(TOKEN_CHAR);
    } else {
        error("'int' or 'char'", current_token.row, current_token.col);
    }}

void identifier_list() {
    match(TOKEN_ID);

    if (current_token.type == TOKEN_COMMA) {
        match(TOKEN_COMMA);
        identifier_list(); 
    }}

void assign_stat() {
    match(TOKEN_ID);
    match(TOKEN_EQUALS);
    
    if (current_token.type == TOKEN_ID) {
        match(TOKEN_ID);
    } else if (current_token.type == TOKEN_NUM) {
        match(TOKEN_NUM);
    } else {
        error("identifier or number", current_token.row, current_token.col);
    }
    
    match(TOKEN_SEMICOLON);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    input_file = fopen(argv[1], "r");
    if (!input_file) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    get_token();
    
    program();
    
    if (current_token.type == TOKEN_EOF) {
        printf("Parsing completed successfully!\n");
    } else {
        printf("Error: Extra tokens after the end of the program\n");
    }
    
    fclose(input_file);
    
    return 0;
}

//main(){int a,b;a=1;}
//main(){int a,b a=1;}
