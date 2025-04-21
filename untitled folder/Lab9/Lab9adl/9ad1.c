#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    SWITCH_TK, LPAREN_TK, RPAREN_TK, LBRACE_TK, RBRACE_TK,
    CASE_TK, DEFAULT_TK, COLON_TK, ASSIGN_TK, SEMICOLON_TK,
    IDENTIFIER_TK, INTEGER_TK, END_OF_TOKENS, INVALID_TK
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
        // Identifier
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

        if (strcmp(token.value, "switch") == 0) token.type = SWITCH_TK;
        else if (strcmp(token.value, "case") == 0) token.type = CASE_TK;
        else if (strcmp(token.value, "default") == 0) token.type = DEFAULT_TK;
        else token.type = IDENTIFIER_TK;
    } else if (isdigit(input_buffer[current_pos])) {
        // Integer
        int start = current_pos;
        while (current_pos < buffer_size && isdigit(input_buffer[current_pos])) {
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
        token.type = INTEGER_TK;
    } else {
        // Single character tokens
        switch (input_buffer[current_pos]) {
            case '(': token.type = LPAREN_TK; break;
            case ')': token.type = RPAREN_TK; break;
            case '{': token.type = LBRACE_TK; break;
            case '}': token.type = RBRACE_TK; break;
            case ':': token.type = COLON_TK; break;
            case '=': token.type = ASSIGN_TK; break;
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
    // Optionally, you might want to continue parsing to find more errors
    exit(EXIT_FAILURE);
}

// Parser functions (one for each non-terminal)
void parse_switch_statement();
void parse_case_list();
void parse_case_statement();
void parse_default_statement();
void parse_statement_list();
void parse_statement();
void parse_expression();
void parse_constant_expression();

void parse_switch_statement() {
    if (current_token.type == SWITCH_TK) {
        consumeToken();
        if (current_token.type == LPAREN_TK) {
            consumeToken();
            parse_expression();
            if (current_token.type == RPAREN_TK) {
                consumeToken();
                if (current_token.type == LBRACE_TK) {
                    consumeToken();
                    parse_case_list();
                    if (current_token.type == RBRACE_TK) {
                        consumeToken();
                        printf("Successfully parsed switch statement.\n");
                        return;
                    } else {
                        error("Expected '}'");
                    }
                } else {
                    error("Expected '{'");
                }
            } else {
                error("Expected ')'");
            }
        } else {
            error("Expected '('");
        }
    } else {
        error("Expected 'switch'");
    }
}

void parse_case_list() {
    while (current_token.type == CASE_TK) {
        parse_case_statement();
    }
    if (current_token.type == DEFAULT_TK) {
        parse_default_statement();
    }
    // case_list can be empty, so no error if '}' is encountered here
}

void parse_case_statement() {
    if (current_token.type == CASE_TK) {
        consumeToken();
        parse_constant_expression();
        if (current_token.type == COLON_TK) {
            consumeToken();
            parse_statement_list();
        } else {
            error("Expected ':' after 'case'");
        }
    } else {
        // This should not happen if parse_case_list is implemented correctly
        error("Internal parser error: Expected 'case'");
    }
}

void parse_default_statement() {
    if (current_token.type == DEFAULT_TK) {
        consumeToken();
        if (current_token.type == COLON_TK) {
            consumeToken();
            parse_statement_list();
        } else {
            error("Expected ':' after 'default'");
        }
    } else {
        // This should not happen if parse_case_list is implemented correctly
        error("Internal parser error: Expected 'default'");
    }
}

void parse_statement_list() {
    while (current_token.type == IDENTIFIER_TK) {
        parse_statement();
    }
    // statement_list can be empty, so no error if 'case', 'default', or '}' is encountered
}

void parse_statement() {
    if (current_token.type == IDENTIFIER_TK) {
        consumeToken();
        if (current_token.type == ASSIGN_TK) {
            consumeToken();
            parse_expression();
            if (current_token.type == SEMICOLON_TK) {
                consumeToken();
            } else {
                error("Expected ';' after assignment");
            }
        } else {
            error("Expected '=' after identifier in assignment");
        }
    } else {
        // This should not happen if parse_statement_list is implemented correctly
        error("Internal parser error: Expected identifier for statement");
    }
}

void parse_expression() {
    if (current_token.type == IDENTIFIER_TK || current_token.type == INTEGER_TK) {
        consumeToken();
    } else {
        error("Expected identifier or integer in expression");
    }
}

void parse_constant_expression() {
    if (current_token.type == INTEGER_TK) {
        consumeToken();
    } else {
        error("Expected integer in constant expression");
    }
}

int main() {
    printf("Enter the switch block (end with Ctrl+D or a newline on an empty line):\n");

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
        parse_switch_statement();
    } else {
        printf("No input provided.\n");
    }

    free(input_buffer);
    return 0;
}


/*
switch (val) {
case 10:
  a = 5;
  b = c;
default:
  d = 123;
}

[21/04/25, 10:17:58 AM] Sushyanth 🙂🙂: }
[21/04/25, 10:17:59 AM] Sushyanth 🙂🙂: switch (value {
case 1:
  err = 1;
}
invalid input same question
*/