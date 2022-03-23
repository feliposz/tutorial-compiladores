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

char Look; /* O caracter lido "antecipadamente" (lookahead) */

#define MAXTOKEN 16
char Token; /* código do token atual */
char TokenText[MAXTOKEN+1]; /* texto do token atual */

/* lê próximo caracter da entrada */
void NextChar()
{
    Look = getchar();
}

/* pula caracteres em branco */
void SkipWhite()
{
    while (isspace(Look))
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
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
}

/* recebe o nome de um identificador ou palavra-chave */
void GetName()
{
    int i;

    SkipWhite();
    if (!isalpha(Look))
        Expected("Identifier or Keyword");
    for (i = 0; isalnum(Look) && i < MAXTOKEN; i++) {
        TokenText[i] = toupper(Look);
        NextChar();
    }
    TokenText[i] = '\0';
    Token = 'x';
}

/* recebe um número inteiro */
void GetNum()
{
    int i;

    SkipWhite();
    if (!isdigit(Look))
        Expected("Integer");
    for (i = 0; isdigit(Look) && i < MAXTOKEN; i++) {
        TokenText[i] = Look;
        NextChar();
    }
    TokenText[i] = '\0';
    Token = '#';
}

/* analisa e traduz um operador */
void GetOp()
{
    SkipWhite();
    Token = Look;
    TokenText[0] = Look;
    TokenText[1] = '\0';
    NextChar();
}

/* pega o próximo Token de entrada */
void NextToken()
{
    SkipWhite();
    if (isalpha(Look))
        GetName();
    else if (isdigit(Look))
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
        printf("Token: %c Value: %s\n", Token, TokenText);
    } while (Token != '.');
}
