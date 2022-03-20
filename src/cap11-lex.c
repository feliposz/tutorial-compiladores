/*
Análise Léxica Revista

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

#define MAXTOKEN 16
char token; /* código do token atual */
char value[MAXTOKEN+1]; /* texto do token atual */

/* lê próximo caracter da entrada */
void nextChar()
{
    look = getchar();
}

/* pula caracteres em branco */
void skipWhite()
{
    while (isspace(look))
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
/* verifica se entrada combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();
}

/* recebe o nome de um identificador ou palavra-chave */
void getName()
{
    int i;

    skipWhite();
    if (!isalpha(look))
        expected("Identifier or Keyword");
    for (i = 0; isalnum(look) && i < MAXTOKEN; i++) {
        value[i] = toupper(look);
        nextChar();
    }
    value[i] = '\0';
    token = 'x';
}

/* recebe um número inteiro */
void getNum()
{
    int i;

    skipWhite();
    if (!isdigit(look))
        expected("Integer");
    for (i = 0; isdigit(look) && i < MAXTOKEN; i++) {
        value[i] = look;
        nextChar();
    }
    value[i] = '\0';
    token = '#';
}

/* analisa e traduz um operador */
void getOp()
{
    skipWhite();
    token = look;
    value[0] = look;
    value[1] = '\0';
    nextChar();
}

/* pega o próximo token de entrada */
void nextToken()
{
    skipWhite();
    if (isalpha(look))
        getName();
    else if (isdigit(look))
        getNum();
    else
        getOp();
}

/* inicialização do compilador */
void init()
{
    nextChar();
}

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    do {
        nextToken();
        printf("Token: %c Value: %s\n", token, value);
    } while (token != '.');
}
