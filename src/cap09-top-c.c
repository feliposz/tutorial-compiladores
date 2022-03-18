/*
Uma visão do alto - Compilador "C"

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

int look; /* O caracter lido "antecipadamente" (lookahead) */
char currentClass; /* classe atual lida por getClass */
char currentType; /* tipo atual lido por getType */
char currentSigned; /* indica se tipo atual é com ou sem sinal */

/* lê próximo caracter da entrada */
void nextChar()
{
    look = getchar();
}

/* inicialização do compilador */
void init()
{
    nextChar();
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

/* recebe uma classe de armazenamento C */
void getClass()
{
    if (look == 'a' || look == 'x' || look == 's') {
        currentClass = look;
        nextChar();
    } else
        currentClass = 'a';
}

/* recebe um tipo de dado C */
void getType()
{
    currentType = ' ';
    if (look == 'u') {
        currentSigned = 'u';
        currentType = 'i';
        nextChar();
    } else {
        currentSigned = 's';
    }
    if (look == 'i' || look == 'l' || look == 'c') {
        currentType = look;
        nextChar();
    }
}

/* analisa uma declaração de função */
void functionDeclaration(char name)
{
    match('(');
    match(')');
    match('{');
    match('}');
    if (currentType == ' ')
        currentType = 'i';
    printf("Class: %c, Sign: %c, Type: %c, Function: %c\n",
        currentClass, currentSigned, currentType, name);
}

/* analisa uma declaração de variável */
void dataDeclaration(char name)
{
    if (currentType == ' ')
        expected("Type declaration");
    for (;;) {
        printf("Class: %c, Sign: %c, Type: %c, Data: %c\n",
            currentClass, currentSigned, currentType, name);
        if (look != ',')
            break;
        match(',');
        name = getName();
    }
    match(';');
}

/* analisa uma declaração */
void topDeclaration()
{
    char name;

    name = getName();
    if (look == '(')
        functionDeclaration(name);
    else
        dataDeclaration(name);
}

/* PROGRAMA PRINCIPAL */
int main()
{
    init();

    while (look != EOF && look != '\n') {
        getClass();
        getType();
        topDeclaration();
    }

    return 0;
}