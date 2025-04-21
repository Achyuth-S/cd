#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 1024

int id_counter = 1;

int is_valid_identifier(const char *s) {
    if (!isalpha(s[0]) && s[0] != '_') return 0;
    for (int i = 1; s[i]; i++) {
        if (!isalnum(s[i]) && s[i] != '_') return 0;
    }
    return 1;
}

void clean_whitespace(char *line) {
    char *start = line;
    while (isspace(*start)) start++;

    char *end = line + strlen(line) - 1;
    while (end > start && isspace(*end)) *end-- = '\0';

    if (start != line)
        memmove(line, start, strlen(start) + 1);
}

void fix_declaration(char *line, int lineno) {
    clean_whitespace(line);
    if (strlen(line) == 0) return;

    if (line[strlen(line) - 1] != ';') {
        printf("[Line %d] Added missing semicolon.\n", lineno);
        strcat(line, ";");
    }

    char fixed[MAX] = "";
    char type[10];
    int i = 0;

    sscanf(line, "%s", type);
    if (strcmp(type, "int") != 0 && strcmp(type, "char") != 0) {
        printf("[Line %d] Unknown type. Skipping correction.\n", lineno);
        return;
    }

    strcat(fixed, type);
    strcat(fixed, " ");

    i = strlen(type);
    while (isspace(line[i])) i++;

    int token_started = 0;

    while (line[i] != '\0' && line[i] != ';') {
        char token[64] = "";
        int k = 0;

        // Collect * or &
        while (line[i] == '*' || line[i] == '&') {
            if (line[i] == '&') {
                printf("[Line %d] Replacing '&' with '*'.\n", lineno);
                token[k++] = '*';
                i++;
            } else {
                token[k++] = line[i++];
            }
        }

        while (isspace(line[i])) i++;

        // Collect identifier
        if (isalpha(line[i]) || line[i] == '_') {
            while (isalnum(line[i]) || line[i] == '_') {
                token[k++] = line[i++];
            }
        } else if (line[i] == ',' || line[i] == ';') {
            printf("[Line %d] Inserted missing identifier.\n", lineno);
            sprintf(token + k, "var%d", id_counter++);
            k = strlen(token);
        }

        token[k] = '\0';

        // Validate identifier after *
        int stars = 0;
        while (token[stars] == '*') stars++;

        if (!is_valid_identifier(token + stars)) {
            printf("[Line %d] Invalid identifier '%s', replaced with 'var%d'\n", lineno, token + stars, id_counter);
            sprintf(token + stars, "var%d", id_counter++);
        }

        if (token_started) strcat(fixed, ", ");
        strcat(fixed, token);
        token_started = 1;

        while (isspace(line[i])) i++;
        if (line[i] == ',') i++;
        while (isspace(line[i])) i++;
    }

    strcat(fixed, ";");
    printf("[Line %d] Corrected: %s\n", lineno, fixed);
}

int main() {
    char buffer[MAX];
    int lineno = 1;

    printf("Enter variable declarations (end input with empty line):\n");
    while (fgets(buffer, sizeof(buffer), stdin)) {
        if (strcmp(buffer, "\n") == 0) break;
        fix_declaration(buffer, lineno++);
    }

    return 0;
}

/*
Enter variable declarations (end input with empty line):
int *p **q
char &a, b
float *x
int , ,y;
int **,z;
[Line 1] Added missing semicolon.
[Line 1] Corrected: int *p, **q;
[Line 2] Added missing semicolon.
[Line 2] Replacing '&' with '*'.
[Line 2] Corrected: char *a, b;
[Line 3] Added missing semicolon.
[Line 3] Unknown type. Skipping correction.
[Line 4] Inserted missing identifier.
[Line 4] Inserted missing identifier.
[Line 4] Corrected: int var1, var2, y;
[Line 5] Inserted missing identifier.
[Line 5] Corrected: int **var3, z;
*/
