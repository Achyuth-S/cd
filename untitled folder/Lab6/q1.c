#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int curr = 0;
char str[100];

void S();
void T();
void invalid();
void valid();

void invalid() {
    printf("-----------------ERROR!----------------\n");
    exit(0);
}

void valid() {
    printf("----------------SUCCESS!---------------\n");
    exit(0);
}

void S() {
    if (str[curr] == 'a') {
        curr++;
    } else if (str[curr] == '>') {
        curr++;
    } else if (str[curr] == '(') {
        curr++;
        T();
        if (str[curr] == ')') {
            curr++;
        } else {
            invalid();
        }
    } else {
        invalid();
    }
}

void T() {
    S();
    if (str[curr] == ',') {
        curr++;
        T();
    }
}

int main() {
    printf("Enter String: ");
    scanf("%s", str); 
    S(); 
    if (str[curr] == '\0') { 
        valid(); 
    } else {
        invalid(); 
    }
    return 0;
}

//Enter String: (a
//-----------------ERROR!----------------

//Enter String: a
//----------------SUCCESS!---------------


