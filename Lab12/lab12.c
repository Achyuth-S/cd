    
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int tempCount = 1;

void generateAssembly(char postfix[]) {
    char stack[100][10];
    int top = -1;
    char op1[10], op2[10], result[10];

    for (int i = 0; postfix[i] != '\0'; i++) {
        char symbol = postfix[i];

        if (isalnum(symbol)) {
            char operand[2] = {symbol, '\0'};
            strcpy(stack[++top], operand);
        } else {
            // Pop operands from the stack
            strcpy(op2, stack[top--]);
            strcpy(op1, stack[top--]);

            // Create temporary register
            sprintf(result, "T%d", tempCount++);

            // Generate assembly
            printf("LOAD %s\n", op1);
            switch (symbol) {
                case '+': printf("ADD %s\n", op2); break;
                case '-': printf("SUB %s\n", op2); break;
                case '*': printf("MUL %s\n", op2); break;
                case '/': printf("DIV %s\n", op2); break;
                default: printf("Unsupported operator %c\n", symbol); return;
            }
            printf("STORE %s\n", result);

            // Push result back to stack
            strcpy(stack[++top], result);
        }
    }

    printf("Result in: %s\n", stack[top]);
}

int main() {
    char postfix[100];
    printf("Enter Postfix Expression (e.g. ab+cd-*): ");
    scanf("%s", postfix);

    printf("\nAssembly Code:\n");
    generateAssembly(postfix);

    return 0;
}
/*
ab-cd+*
LOAD a
SUB b
STORE T1
LOAD c
ADD d
STORE T2
LOAD T1
MUL T2
STORE T3
Result in: T3*/