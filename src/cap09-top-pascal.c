/*
Uma visão do alto - Compilador "Pascal"

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

/* funções de declaração pascal */

void labels()
{
    match('l');
}

void constants()
{
    match('c');
}

void types()
{
    match('t');
}

void variables()
{
    match('v');
}

void procedure()
{
    match('p');
}

void function()
{
    match('f');
}

/* analisa e traduz declarações pascal */
void declarations()
{
    int valid;

    do {
        valid = 1;
        switch (look) {
            case 'l': labels(); break;
            case 'c': constants(); break;
            case 't': types(); break;
            case 'v': variables(); break;
            case 'p': procedure(); break;
            case 'f': function(); break;
            default: valid = 0; break;
        }
    } while (valid);
}

/* analisa e traduz um bloco de comandos pascal */
void statements()
{
    match('b');
    while (look != 'e')
        nextChar();
    match('e');
}

/* analisa e traduz um bloco Pascal */
void block(char name)
{
    declarations();
    printf("%c:\n", name);
    statements();
}

/* emite código para o prólogo de um programa */
void prolog()
{
    printf("\t.model small\n");
    printf("\t.stack\n");
    printf("\t.code\n");
    printf("PROG segment byte public\n");
    printf("\tassume cs:PROG,ds:PROG,es:PROG,ss:PROG\n");
}

/* emite código para o epílogo de um programa */
void epilog(char name)
{
    printf("exit_prog:\n");
    printf("\tMOV AX,4C00h  ; AH=4C (termina execucao do programa) AL=00 (saida ok)\n");
    printf("\tINT 21h       ; chamada de sistema DOS\n");
    printf("PROG ends\n");
    printf("\tend %c\n", name);
}

/* analisa e traduz um programa Pascal */
void program()
{
    char name;

    match('p'); /* trata do cabeçalho do programa */
    name = getName();
    prolog();
    block(name);
    match('.');
    epilog(name);
}

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    program();

    return 0;
}