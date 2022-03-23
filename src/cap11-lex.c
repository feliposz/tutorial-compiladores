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
void NextChar()
{
    look = getchar();
}

/* pula caracteres em branco */
void SkipWhite()
{
    while (isspace(look))
        NextChar();
}

/* exibe uma mensagem de erro formatada */
void Error(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
}

/* exibe uma mensagem de erro formatada e sai */
void Abort(char *fmt, ...)
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
void Expected(char *fmt, ...)
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
void EmitLn(char *fmt, ...)
{
    va_list args;

    putchar('\t');

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    putchar('\n');
}
/* verifica se entrada combina com o esperado */
void Match(char c)
{
    if (look != c)
        Expected("'%c'", c);
    NextChar();
}

/* recebe o nome de um identificador ou palavra-chave */
void GetName()
{
    int i;

    SkipWhite();
    if (!isalpha(look))
        Expected("Identifier or Keyword");
    for (i = 0; isalnum(look) && i < MAXTOKEN; i++) {
        value[i] = toupper(look);
        NextChar();
    }
    value[i] = '\0';
    token = 'x';
}

/* recebe um número inteiro */
void GetNum()
{
    int i;

    SkipWhite();
    if (!isdigit(look))
        Expected("Integer");
    for (i = 0; isdigit(look) && i < MAXTOKEN; i++) {
        value[i] = look;
        NextChar();
    }
    value[i] = '\0';
    token = '#';
}

/* analisa e traduz um operador */
void GetOp()
{
    SkipWhite();
    token = look;
    value[0] = look;
    value[1] = '\0';
    NextChar();
}

/* pega o próximo token de entrada */
void NextToken()
{
    SkipWhite();
    if (isalpha(look))
        GetName();
    else if (isdigit(look))
        GetNum();
    else
        GetOp();
}

/* inicialização do compilador */
void Init()
{
    NextChar();
}

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    do {
        NextToken();
        printf("Token: %c Value: %s\n", token, value);
    } while (token != '.');
}
