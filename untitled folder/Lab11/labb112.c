#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXTOK 10000
#define MAXLEX 64

typedef enum {
    T_ID, T_NUM,
    T_IF, T_ELSE,
    T_SWITCH, T_CASE, T_DEFAULT, T_BREAK,
    T_ASSIGN, T_SEMI, T_COLON,
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,
    T_PLUS, T_MINUS, T_MUL, T_DIV,
    T_LT, T_GT, T_LE, T_GE, T_EQ, T_NE,
    T_EOF
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAXLEX];
} Token;

Token tokens[MAXTOK];
int ntok=0, pos=0;


int tcnt=0, lcnt=0;
char *newTemp() {
    static char buf[16];
    sprintf(buf,"t%d",tcnt++);
    return strdup(buf);
}
char *newLabel() {
    static char buf[16];
    sprintf(buf,"L%d",lcnt++);
    return strdup(buf);
}


void tokenize(const char *s) {
    int i=0;
    while (s[i]) {
        if (isspace(s[i])) { i++; continue; }

        if (isalpha(s[i])) {
            char buf[MAXLEX]; int j=0;
            while (isalnum(s[i])) buf[j++]=s[i++];
            buf[j]=0;
            TokenType tt=T_ID;
            if (!strcmp(buf,"if"))     tt=T_IF;
            else if (!strcmp(buf,"else"))   tt=T_ELSE;
            else if (!strcmp(buf,"switch")) tt=T_SWITCH;
            else if (!strcmp(buf,"case"))   tt=T_CASE;
            else if (!strcmp(buf,"default"))tt=T_DEFAULT;
            else if (!strcmp(buf,"break"))  tt=T_BREAK;
            tokens[ntok].type=tt;
            strcpy(tokens[ntok].lexeme, buf);
            ntok++; continue;
        }

        if (isdigit(s[i])) {
            char buf[MAXLEX]; int j=0;
            while (isdigit(s[i])) buf[j++]=s[i++];
            buf[j]=0;
            tokens[ntok].type=T_NUM;
            strcpy(tokens[ntok].lexeme, buf);
            ntok++; continue;
        }


        if ((s[i]=='<'||s[i]=='>'||s[i]=='='||s[i]=='!') && s[i+1]=='=') {
            char op[3]={s[i],s[i+1],0};
            TokenType tt = !strcmp(op,"<=")?T_LE:
                           !strcmp(op,">=")?T_GE:
                           !strcmp(op,"==")?T_EQ:
                                            T_NE;
            tokens[ntok].type=tt;
            strcpy(tokens[ntok].lexeme, op);
            ntok++; i+=2; continue;
        }


        TokenType tt=T_EOF;
        char lex[2]={s[i],0};
        switch(s[i]) {
            case '=': tt=T_ASSIGN; break;
            case ';': tt=T_SEMI;   break;
            case ':': tt=T_COLON;  break;
            case '(': tt=T_LPAREN; break;
            case ')': tt=T_RPAREN; break;
            case '{': tt=T_LBRACE; break;
            case '}': tt=T_RBRACE; break;
            case '+': tt=T_PLUS;   break;
            case '-': tt=T_MINUS;  break;
            case '*': tt=T_MUL;    break;
            case '/': tt=T_DIV;    break;
            case '<': tt=T_LT;     break;
            case '>': tt=T_GT;     break;
            default:
                fprintf(stderr,"Unknown '%c'\n",s[i]);
                exit(1);
        }
        tokens[ntok].type=tt;
        strcpy(tokens[ntok].lexeme, lex);
        ntok++; i++;
    }
    tokens[ntok].type=T_EOF;
    strcpy(tokens[ntok++].lexeme,"EOF");
}

Token *peek(){ return &tokens[pos]; }
Token *get(){ return &tokens[pos++]; }

char *parseExpression();
void parseStatement();
void parseStatementList();

char *parseExpression(){
    if (peek()->type!=T_ID && peek()->type!=T_NUM) {
        fprintf(stderr,"Expr: expected id/num, found '%s'\n", peek()->lexeme);
        exit(1);
    }
    char *left=newTemp();
    printf("%s = %s\n", left, peek()->lexeme);
    get();

    while (peek()->type==T_PLUS || peek()->type==T_MINUS ||
           peek()->type==T_MUL  || peek()->type==T_DIV   ||
           peek()->type==T_LT   || peek()->type==T_GT    ||
           peek()->type==T_LE   || peek()->type==T_GE    ||
           peek()->type==T_EQ   || peek()->type==T_NE) {

        char *opstr = peek()->lexeme;
        get();

        if (peek()->type!=T_ID && peek()->type!=T_NUM) {
            fprintf(stderr,"Expr: expected id/num after '%s'\n", opstr);
            exit(1);
        }
        char *rlex = strdup(peek()->lexeme);
        get();

        char *res=newTemp();
        printf("%s = %s %s %s\n", res, left, opstr, rlex);
        left = res;
    }
    return left;
}

void parseAssign(int needSemi){
    if (peek()->type!=T_ID) { fprintf(stderr,"Assign: expected id\n"); exit(1); }
    char id[MAXLEX]; strcpy(id, peek()->lexeme);
    get();
    if (peek()->type!=T_ASSIGN) { fprintf(stderr,"Assign: expected '='\n"); exit(1); }
    get();
    char *r = parseExpression();
    if (needSemi) {
        if (peek()->type!=T_SEMI) { fprintf(stderr,"Assign: expected ';'\n"); exit(1); }
        get();
    }
    printf("%s = %s\n", id, r);
}


void parseIfElse(){
    get(); 
    if (peek()->type!=T_LPAREN) exit(1);
    get(); 
    char *cond = parseExpression();
    if (peek()->type!=T_RPAREN) exit(1);
    get(); 

    char *elseL=newLabel(), *endL=newLabel();
    printf("if %s == 0 goto %s\n", cond, elseL);

    if (peek()->type==T_LBRACE) { get(); parseStatementList(); get(); }
    else parseStatement();

    printf("goto %s\n", endL);
    printf("%s:\n", elseL);

    if (peek()->type==T_ELSE) {
        get();
        if (peek()->type==T_LBRACE) { get(); parseStatementList(); get(); }
        else parseStatement();
    }
    printf("%s:\n", endL);
}


void parseSwitch(){
    get(); // switch
    if (peek()->type!=T_LPAREN) exit(1);
    get(); // (
    char *expr = parseExpression();
    if (peek()->type!=T_RPAREN) exit(1);
    get(); // )
    if (peek()->type!=T_LBRACE) exit(1);
    get(); // {

    char *endL = newLabel();
    struct { char *val,*lbl; } cases[20];
    int nc=0;

    while (peek()->type==T_CASE) {
        get(); // case
        if (peek()->type!=T_NUM) exit(1);
        cases[nc].val = strdup(peek()->lexeme);
        cases[nc].lbl = newLabel();
        get(); // num
        if (peek()->type!=T_COLON) exit(1);
        get(); // :

        while (peek()->type!=T_BREAK) parseStatement();
        get(); // break
        if (peek()->type!=T_SEMI) exit(1);
        get(); // ;
        nc++;
    }

    char *defL = endL;
    if (peek()->type==T_DEFAULT) {
        get(); // default
        if (peek()->type!=T_COLON) exit(1);
        get(); // :
        defL = newLabel();
        // skip until break
        while (peek()->type!=T_BREAK) parseStatement();
        get(); // break
        if (peek()->type!=T_SEMI) exit(1);
        get(); // ;
    }

    for (int i=0;i<nc;i++)
        printf("if %s == %s goto %s\n", expr, cases[i].val, cases[i].lbl);
    printf("goto %s\n", defL);

    for (int i=0;i<nc;i++)
        printf("%s:\n", cases[i].lbl);

    if (defL!=endL)
        printf("%s:\n", defL);

    if (peek()->type!=T_RBRACE) exit(1);
    get(); // }
    printf("%s:\n", endL);
}


void parseStatement(){
    if (peek()->type==T_IF)      parseIfElse();
    else if (peek()->type==T_SWITCH) parseSwitch();
    else                          parseAssign(1);
}

void parseStatementList(){
    while (peek()->type!=T_RBRACE && peek()->type!=T_EOF)
        parseStatement();
}

int main(){
    static char src[20000], *p=src;
    while (fgets(p, sizeof(src)-(p-src), stdin)) p+=strlen(p);

    tokenize(src);
    parseStatementList();
    return 0;
}
