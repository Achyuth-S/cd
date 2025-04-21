
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tempCount = 0, labelCount = 0;

char* newTemp() {
    char* temp = (char*)malloc(10);
    sprintf(temp, "t%d", tempCount++);
    return temp;
}

char* newLabel() {
    char* label = (char*)malloc(10);
    sprintf(label, "L%d", labelCount++);
    return label;
}

void generateIf(char* condition, char* statement) {
    char* label = newLabel();
    printf("if %s goto %s\n", condition, label);
    printf("goto next\n");
    printf("%s:\n", label);
    printf("%s\n", statement);
    printf("next:\n");
}

void generateIfElse(char* condition, char* stmt1, char* stmt2) {
    char *L1 = newLabel(), *L2 = newLabel();
    printf("if %s goto %s\n", condition, L1);
    printf("goto %s\n", L2);
    printf("%s:\n", L1);
    printf("%s\n", stmt1);
    printf("goto next\n");
    printf("%s:\n", L2);
    printf("%s\n", stmt2);
    printf("next:\n");
}

void generateWhile(char* condition, char* stmt) {
    char *start = newLabel(), *trueLabel = newLabel(), *end = newLabel();
    printf("%s:\n", start);
    printf("if %s goto %s\n", condition, trueLabel);
    printf("goto %s\n", end);
    printf("%s:\n", trueLabel);
    printf("%s\n", stmt);
    printf("goto %s\n", start);
    printf("%s:\n", end);
}

void trim(char* str) {
    int i;
    for (i = 0; str[i]; i++) {
        if (str[i] == '\n') str[i] = '\0';
    }
}

int main() {
    char input[256], condition[100], stmt1[100], stmt2[100];

    printf("Enter your statement:\n");
    fgets(input, sizeof(input), stdin);
    trim(input);

    if (strncmp(input, "if", 2) == 0) {
        if (strstr(input, "else")) {
            sscanf(input, "if (%[^)]) %[^;]; else %[^;];", condition, stmt1, stmt2);
            generateIfElse(condition, stmt1, stmt2);
        } else {
            sscanf(input, "if (%[^)]) %[^;];", condition, stmt1);
            generateIf(condition, stmt1);
        }
    } else if (strncmp(input, "while", 5) == 0) {
        sscanf(input, "while (%[^)]) %[^;];", condition, stmt1);
        generateWhile(condition, stmt1);
    } else {
        printf("Unsupported or invalid input format.\n");
    }

    return 0;
}
/*input:
if (a > b) x = x - 1; else x = x + 1;
if a > b goto L0
goto L1
L0:
x = x - 1;
goto next
L1:
x = x + 1;
next:
*/


