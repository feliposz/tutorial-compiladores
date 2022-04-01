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

char Look; /* O caractere lido "antecipadamente" (lookahead) */

#define MAXTOKEN 16
char Token; /* Código do token atual */
char TokenText[MAXTOKEN+1]; /* Texto do token atual */

/* Lê próximo caractere da entrada */
void NextChar()
{
    Look = getchar();
}

/* Pula caracteres em branco */
void SkipWhite()
{
    while (isspace(Look))
        NextChar();
}

/* Exibe uma mensagem de erro formatada */
void Error(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
}

/* Exibe uma mensagem de erro formatada e sai */
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

/* Alerta sobre alguma entrada esperada */
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


/* Emite uma instrução seguida por uma nova linha */
void EmitLn(char *fmt, ...)
{
    va_list args;

    putchar('\t');

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    putchar('\n');
}
/* Verifica se entrada combina com o esperado */
void Match(char c)
{
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
}

/* Recebe o nome de um identificador ou palavra-chave */
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

/* Recebe um número inteiro */
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

/* Analisa e traduz um operador */
void GetOp()
{
    SkipWhite();
    Token = Look;
    TokenText[0] = Look;
    TokenText[1] = '\0';
    NextChar();
}

/* Pega o próximo Token de entrada */
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

/* Inicialização do compilador */
void Init()
{
    NextChar();
}

/* Programa principal */
int main()
{
    Init();
    do {
        NextToken();
        printf("Token: %c Value: %s\n", Token, TokenText);
    } while (Token != '.');
}
