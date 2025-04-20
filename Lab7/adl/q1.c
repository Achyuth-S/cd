#include <stdio.h>
#include <string.h>
#include <ctype.h>

void parse_pointer_decl(const char* str) {
    printf("Parsing: %s\n", str);
    char type[10], var[100];
    int i = 0;

    // Get the data type
    sscanf(str, "%s", type);
    i += strlen(type) + 1;

    // Check valid type
    if (strcmp(type, "int") != 0 && strcmp(type, "char") != 0) {
        printf("Error: Unknown type '%s'\n", type);
        return;
    }

    while (str[i] != '\0') {
        int ptr_count = 0;
        while (str[i] == '*') {
            ptr_count++;
            i++;
        }

        // Skip whitespace
        while (str[i] == ' ') i++;

        // Read variable name
        int j = 0;
        while (isalnum(str[i])) {
            var[j++] = str[i++];
        }
        var[j] = '\0';

        if (j == 0) {
            printf("Error: Invalid or missing identifier\n");
            return;
        }

        printf("Variable: %s, Type: %s, Pointers: %d\n", var, type, ptr_count);

        while (str[i] == ' ') i++;
        if (str[i] == ',') i++;
        else if (str[i] == ';') break;
        else i++;
    }
}

int main() {
    char input[256];
    printf("Enter pointer declaration (e.g., int *a, **b;):\n");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0; // remove newline
    parse_pointer_decl(input);
    return 0;
}