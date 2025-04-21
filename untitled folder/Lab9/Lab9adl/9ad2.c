#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    STRUCT_TK, LBRACE_TK, RBRACE_TK, SEMICOLON_TK,
    INT_TK, CHAR_TK, FLOAT_TK,
    IDENTIFIER_TK, END_OF_TOKENS, INVALID_TK
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
} Token;

// Lexer state
char* input_buffer = NULL;
int buffer_size = 0;
int current_pos = 0;

// Function to get the next token
Token getNextToken() {
    while (current_pos < buffer_size && isspace(input_buffer[current_pos])) {
        current_pos++;
    }

    if (current_pos >= buffer_size) {
        return (Token){END_OF_TOKENS, NULL};
    }

    Token token;
    if (isalpha(input_buffer[current_pos])) {
        // Identifier or keyword
        int start = current_pos;
        while (current_pos < buffer_size && isalnum(input_buffer[current_pos])) {
            current_pos++;
        }
        int length = current_pos - start;
        token.value = (char*)malloc(length + 1);
        if (token.value == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        strncpy(token.value, input_buffer + start, length);
        token.value[length] = '\0';

        if (strcmp(token.value, "struct") == 0) token.type = STRUCT_TK;
        else if (strcmp(token.value, "int") == 0) token.type = INT_TK;
        else if (strcmp(token.value, "char") == 0) token.type = CHAR_TK;
        else if (strcmp(token.value, "float") == 0) token.type = FLOAT_TK;
        else token.type = IDENTIFIER_TK;
    } else {
        // Single character tokens
        switch (input_buffer[current_pos]) {
            case '{': token.type = LBRACE_TK; break;
            case '}': token.type = RBRACE_TK; break;
            case ';': token.type = SEMICOLON_TK; break;
            default: token.type = INVALID_TK; break;
        }
        token.value = (char*)malloc(2);
        if (token.value == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        token.value[0] = input_buffer[current_pos];
        token.value[1] = '\0';
        current_pos++;
    }
    return token;
}

// Parser state
Token current_token;

// Function to advance to the next token
void consumeToken() {
    if (current_token.type != END_OF_TOKENS) {
        free(current_token.value);
        current_token = getNextToken();
    }
}

// Error handling
void error(const char* message) {
    fprintf(stderr, "Error at position %d: %s. Found token: ", current_pos, message);
    if (current_token.value) {
        fprintf(stderr, "%s (%d)\n", current_token.value, current_token.type);
    } else {
        fprintf(stderr, "END_OF_TOKENS\n");
    }
    exit(EXIT_FAILURE);
}

// Parser functions (one for each non-terminal)
void parse_struct_declaration();
void parse_member_list();
void parse_member_declaration();
void parse_type_specifier();
void parse_identifier();

void parse_struct_declaration() {
    if (current_token.type == STRUCT_TK) {
        consumeToken();
        parse_identifier();
        if (current_token.type == LBRACE_TK) {
            consumeToken();
            parse_member_list();
            if (current_token.type == RBRACE_TK) {
                consumeToken();
                if (current_token.type == SEMICOLON_TK) {
                    consumeToken();
                    printf("Successfully parsed struct declaration.\n");
                    return;
                } else {
                    error("Expected ';' at the end of struct declaration");
                }
            } else {
                error("Expected '}' after member list");
            }
        } else {
            error("Expected '{' after struct name");
        }
    } else {
        error("Expected 'struct' keyword");
    }
}

void parse_member_list() {
    while (current_token.type == INT_TK || current_token.type == CHAR_TK || current_token.type == FLOAT_TK) {
        parse_member_declaration();
    }
    // member_list can be empty, so no error if '}' is encountered
}

void parse_member_declaration() {
    parse_type_specifier();
    parse_identifier();
    if (current_token.type == SEMICOLON_TK) {
        consumeToken();
    } else {
        error("Expected ';' after member declaration");
    }
}

void parse_type_specifier() {
    if (current_token.type == INT_TK || current_token.type == CHAR_TK || current_token.type == FLOAT_TK) {
        printf("Parsed type specifier: %s\n", current_token.value);
        consumeToken();
    } else {
        error("Expected 'int', 'char', or 'float'");
    }
}

void parse_identifier() {
    if (current_token.type == IDENTIFIER_TK) {
        printf("Parsed identifier: %s\n", current_token.value);
        consumeToken();
    } else {
        error("Expected identifier");
    }
}

int main() {
    printf("Enter the struct declaration (end with Ctrl+D or a newline on an empty line):\n");

    // Dynamically allocate memory for the input buffer
    buffer_size = 128;
    input_buffer = (char*)malloc(buffer_size);
    if (input_buffer == NULL) {
        perror("Memory allocation failed");
        return 1;
    }
    input_buffer[0] = '\0'; // Initialize as an empty string

    int current_length = 0;
    char line[128];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        int line_length = strlen(line);
        if (line_length > 0 && line[line_length - 1] == '\n') {
            line[line_length - 1] = '\0'; // Remove trailing newline
            line_length--;
        }

        if (line_length == 0) {
            break; // Stop reading on an empty line
        }

        // Reallocate buffer if needed
        if (current_length + line_length + 1 > buffer_size) {
            buffer_size *= 2;
            char* temp_buffer = (char*)realloc(input_buffer, buffer_size);
            if (temp_buffer == NULL) {
                perror("Memory reallocation failed");
                free(input_buffer);
                return 1;
            }
            input_buffer = temp_buffer;
        }

        strcat(input_buffer + current_length, line);
        current_length += line_length;
    }

    if (ferror(stdin)) {
        perror("Error reading input");
        free(input_buffer);
        return 1;
    }

    if (input_buffer[0] != '\0') {
        current_pos = 0;
        current_token = getNextToken(); // Initialize the first token
        parse_struct_declaration();
    } else {
        printf("No input provided.\n");
    }

    free(input_buffer);
    return 0;
}


/*
Enter the struct declaration (end with Ctrl+D or a newline on an empty line):
struct Employee {
  int id ;
  char name ;
  float salary ;
};

Parsed identifier: Employee
Parsed type specifier: int
Parsed identifier: id
Parsed type specifier: char
Parsed identifier: name
Parsed type specifier: float
Parsed identifier: salary
Successfully parsed struct declaration.
*/