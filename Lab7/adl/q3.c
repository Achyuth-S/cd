#include <stdio.h>
#include <string.h>
#include <ctype.h>

int is_valid_type(const char *type) {
    return strcmp(type, "int") == 0 || strcmp(type, "char") == 0;
}

void correct_line(char *line, FILE *out) {
    char type[10], token[100], corrected[256] = "";
    int i = 0;

    sscanf(line, "%s", type);
    i += strlen(type) + 1;

    // Correct invalid type
    if (!is_valid_type(type)) {
        strcat(corrected, "int ");
    } else {
        strcat(corrected, type);
        strcat(corrected, " ");
    }

    int len = strlen(line);
    while (line[i] && line[i] != ';' && line[i] != '\n') {
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

        // Fix invalid identifier
        if (!isalpha(token[0])) strcpy(token, "var");

        for (int k = 0; k < ptr_count; k++) strcat(corrected, "*");
        strcat(corrected, token);

        while (line[i] == ' ') i++;
        if (line[i] == ',') {
            strcat(corrected, ", ");
            i++;
        }
    }

    // Add semicolon if missing
    strcat(corrected, ";");

    fprintf(out, "%s\n", corrected);
}

int main() {
    FILE *in = fopen("input.c", "r");
    FILE *out = fopen("corrected.c", "w");

    if (!in || !out) {
        printf("Error: Cannot open file\n");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), in)) {
        correct_line(line, out);
    }

    fclose(in);
    fclose(out);

    printf("Correction complete. See corrected.c\n");
    return 0;
}