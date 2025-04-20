#include <stdio.h>
#include <string.h>
#include <ctype.h>

int is_valid_type(const char *type) {
    return strcmp(type, "int") == 0 || strcmp(type, "char") == 0;
}

int is_valid_identifier(const char *id) {
    if (!isalpha(id[0])) return 0;
    for (int i = 1; id[i]; i++) {
        if (!isalnum(id[i])) return 0;
    }
    return 1;
}

void check_line(char *line, int line_no) {
    char type[10], token[100];
    int i = 0;

    // Extract type
    sscanf(line, "%s", type);
    i += strlen(type) + 1;

    if (!is_valid_type(type)) {
        printf("Line %d: Error - Invalid data type '%s'\n", line_no, type);
        return;
    }

    int len = strlen(line);
    if (line[len - 1] != ';') {
        printf("Line %d: Error - Missing semicolon\n", line_no);
    }

    while (line[i] && line[i] != ';') {
        int ptr_count = 0;
        while (line[i] == '*') {
            ptr_count++;
            i++;
        }

        while (line[i] == ' ') i++;

        int j = 0;
        while (isalnum(line[i])) {
            token[j++] = line[i++];
        }
        token[j] = '\0';

        if (!is_valid_identifier(token)) {
            printf("Line %d: Error - Invalid identifier '%s'\n", line_no, token);
            return;
        }

        while (line[i] == ' ') i++;
        if (line[i] == ',') i++;
    }
}

int main() {
    FILE *file = fopen("input.c", "r");
    if (!file) {
        printf("Error: Cannot open input.c\n");
        return 1;
    }

    char line[256];
    int line_no = 1;
    while (fgets(line, sizeof(line), file)) {
        check_line(line, line_no++);
    }

    fclose(file);
    return 0;
}