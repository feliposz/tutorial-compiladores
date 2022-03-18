/*
Expressões Booleanas - Parte 1 (até antes de "Adicionando estruturas de controle")

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */
int labelCount; /* Contador usado pelo gerador de rótulos */

/* protótipos */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName();
char getNum();
void emit(char *fmt, ...);

int isAddOp(char c);
int isMulOp(char c);
int isOrOp(char c);
int isRelOp(char c);
int isBoolean(char c);
int getBoolean();

int newLabel();
void postLabel(int lbl);

void boolFactor();
void notFactor();
void boolTerm();
void boolOr();
void boolXor();
void boolExpression();
void equals();
void notEquals();
void greater();
void less();
void relation();
void ident();
void factor();
void signedFactor();
void multiply();
void divide();
void term();
void add();
void subtract();
void expression();

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    boolExpression();
    return 0;
}

/* inicialização do compilador */
void init()
{
    nextChar();
}

/* lê próximo caracter da entrada */
void nextChar()
{
    look = getchar();
}

/* exibe uma mensagem de erro formatada */
void error(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
}

/* exibe uma mensagem de erro formatada e sai */
void fatal(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);

    exit(1);
}

/* alerta sobre alguma entrada esperada */
void expected(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputs(" expected!\n", stderr);

    exit(1);
}

/* verifica se entrada combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();
}

/* recebe o nome de um identificador */
char getName()
{
    char name;

    if (!isalpha(look))
        expected("Name");
    name = toupper(look);
    nextChar();

    return name;
}

/* recebe um número inteiro */
char getNum()
{
    char num;

    if (!isdigit(look))
        expected("Integer");
    num = look;
    nextChar();

    return num;
}

/* emite uma instrução seguida por uma nova linha */
void emit(char *fmt, ...)
{
    va_list args;

    putchar('\t');

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    putchar('\n');
}

/* reconhece operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* reconhece operador multiplicativo */
int isMulOp(char c)
{
    return (c == '*' || c == '/');
}

/* reconhece operador relacional */
int isRelOp(char c)
{
    return (c == '=' || c == '#' || c == '>' || c == '<');
}

/* reconhece uma literal Booleana */
int isBoolean(char c)
{
    return (c == 'T' || c == 'F');
}

/* reconhece um operador OU */
int isOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* recebe uma literal Booleana */
int getBoolean()
{
    int boolean;

    if (!isBoolean(look))
        expected("Boolean Literal");
    boolean = (look == 'T');
    nextChar();

    return boolean;
}

/* gera um novo rótulo único */
int newLabel()
{
    return labelCount++;
}

/* emite um rótulo */
void postLabel(int lbl)
{
    printf("L%d:\n", lbl);
}

/* analisa e traduz um fator booleano */
void boolFactor()
{
    if (isBoolean(look)) {
        if (getBoolean())
            emit("MOV AX, -1");
        else
            emit("MOV AX, 0");
    } else
        relation();
}

/* analisa e traduz um fator booleno com NOT opcional */
void notFactor()
{
    if (look == '!') {
        match('!');
        boolFactor();
        emit("NOT AX");
    } else
        boolFactor();
}

/* analisa e traduz um termo booleano*/
void boolTerm()
{
    notFactor();
    while (look == '&') {
        emit("PUSH AX");
        match('&');
        notFactor();
        emit("POP BX");
        emit("AND AX, BX");
    }
}

/* reconhece e traduz um operador OR */
void boolOr()
{
    match('|');
    boolTerm();
    emit("POP BX");
    emit("OR AX, BX");
}

/* reconhece e traduz um operador XOR */
void boolXor()
{
    match('~');
    boolTerm();
    emit("POP BX");
    emit("XOR AX, BX");
}

/* analisa e traduz uma expressão booleana */
void boolExpression()
{
    boolTerm();
    while (isOrOp(look)) {
        emit("PUSH AX");
        switch (look) {
          case '|':
              boolOr();
              break;
          case '~' :
              boolXor();
              break;
        }
    }
}

/* reconhece e traduz um operador de igualdade */
void equals()
{
    int l1, l2;

    match('=');
    l1 = newLabel();
    l2 = newLabel();
    expression();
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JE L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* reconhece e traduz um operador de não-igualdade */
void notEquals()
{
    int l1, l2;

    match('#');
    l1 = newLabel();
    l2 = newLabel();
    expression();
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JNE L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* reconhece e traduz um operador de maior que */
void greater()
{
    int l1, l2;

    match('>');
    l1 = newLabel();
    l2 = newLabel();
    expression();
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JG L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* reconhece e traduz um operador de menor que */
void less()
{
    int l1, l2;

    match('<');
    l1 = newLabel();
    l2 = newLabel();
    expression();
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JL L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* analisa e traduz uma relação */
void relation()
{
    expression();
    if (isRelOp(look)) {
        emit("PUSH AX");
        switch (look) {
            case '=':
                equals();
                break;
            case '#':
                notEquals();
                break;
            case '>':
                greater();
                break;
            case '<':
                less();
                break;
        }
    }
}

/* analisa e traduz um identificador */
void ident()
{
    char name;

    name = getName();
    if (look == '(') {
        match('(');
        match(')');
        emit("CALL %c", name);
    } else
        emit("MOV AX, [%c]", name);
}

/* analisa e traduz um fator matemático */
void factor()
{
    if (look == '(') {
        match('(');
        boolExpression();
        match(')');
    } else if(isalpha(look))
        ident();
    else
        emit("MOV AX, %c", getNum());
}

/* analisa e traduz um fator com sinal opcional */
void signedFactor()
{
    int minusSign = (look == '-');
    if (isAddOp(look))
    {
        nextChar();
        skipWhite();
    }
    factor();
    if (minusSign)
        emit("NEG AX");
}

/* reconhece e traduz uma multiplicação */
void multiply()
{
    match('*');
    factor();
    emit("POP BX");
    emit("IMUL BX");
}

/* reconhece e traduz uma divisão */
void divide()
{
    match('/');
    factor();
    emit("POP BX");
    emit("XCHG AX, BX");
    emit("CWD");
    emit("IDIV BX");
}

/* analisa e traduz um termo matemático */
void term()
{
    signedFactor();
    while (isMulOp(look)) {
        emit("PUSH AX");
        switch(look) {
            case '*':
                multiply();
                break;
            case '/':
                divide();
                break;
        }
    }
}

/* reconhece e traduz uma soma */
void add()
{
    match('+');
    term();
    emit("POP BX");
    emit("ADD AX, BX");
}

/* reconhece e traduz uma subtração */
void subtract()
{
    match('-');
    term();
    emit("POP BX");
    emit("SUB AX, BX");
    emit("NEG AX");
}

/* analisa e traduz uma expressão matemática */
void expression()
{
    term();
    while (isAddOp(look)) {
        emit("PUSH AX");
        switch(look) {
            case '+':
                add();
                break;
            case '-':
                subtract();
                break;
        }
    }
}
