/*
Juntando os analisadores: Primeiro passo - Sub-conjunto

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
void newLine();
void skipWhite();

int isAddOp(char c);
int isMulOp(char c);

int newLabel();
void postLabel(int lbl);

void ident();
void factor();
void signedFactor();
void add();
void subtract();
void multiply();
void divide();
void termCommon();
void firstTerm();
void term();
void expression();
void assignment();
void condition();

void doIf();
void block();
void program();

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    program();

    return 0;
}

/* inicialização do compilador */
void init()
{
    labelCount = 0;
    nextChar();
    skipWhite();
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
    skipWhite();
}

/* recebe o nome de um identificador */
char getName()
{
    char name;

    while (look == '\n')
        newLine();
    if (!isalpha(look))
        expected("Name");
    name = toupper(look);
    nextChar();
    skipWhite();

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
    skipWhite();

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

/* reconhece uma linha em branco */
void newLine()
{
    if (look == '\n')
        nextChar();
}

/* pula caracteres de espaço */
void skipWhite()
{
    while (look == ' ' || look == '\t')
        nextChar();
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

/* analisa e traduz um comando de atribuição */
void assignment()
{
    char name;

    name = getName();
    match('=');
    expression();
    emit("MOV [%c], AX", name);
}

/* analisa e traduz um fator matemático */
void factor()
{
    if (look == '(') {
        match('(');
        expression();
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

/* código comum a firstTerm e term */
void termCommon()
{
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

/* analisa e traduz um termo matemático no início de uma expressão */
void firstTerm()
{
    signedFactor();
    termCommon();
}

/* analisa e traduz um termo matemático */
void term()
{
    factor();
    termCommon();
}

/* analisa e traduz uma expressão matemática */
void expression()
{
    firstTerm();
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

/* analisa e traduz uma condição */
void condition()
{
    emit("; condition");
}

void doIf()
{
    int l1, l2;

    match('i');
    condition();
    l1 = newLabel();
    l2 = l1;
    emit("JZ L%d", l1);
    block();
    if (look == 'l') {
        match('l');
        l2 = newLabel();
        emit("JMP L%d", l2);
        postLabel(l1);
        block();
    }
    match('e');
    postLabel(l2);
}

/* analisa e traduz um bloco de comandos */
void block()
{
    int follow;

    follow = 0;

    while (!follow) {
        switch (look) {
            case 'i':
                doIf();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            case '\n':
                newLine();
                break;
            default:
                assignment(); 
                break;
        }
    }
}

/* analisa e traduz um programa completo */
void program()
{
    block();
    if (look != 'e')
        expected("End");
    emit("; END");
}