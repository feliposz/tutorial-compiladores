/*
Tipos de variáveis - Base para experiências

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

#define SYMBOLTABLE_SIZE 26
char SymbolTable[SYMBOLTABLE_SIZE]; /* Tabela de símbolos */

/* Rotinas utilitárias */
void Init();
void NextChar();
void Error(char *fmt, ...);
void Abort(char *fmt, ...);
void Expected(char *fmt, ...);
void Unrecognized(char name);
void EmitLn(char *fmt, ...);

/* Tratamento da tabela de símbolos */
void DumpTable();

/* Analisador léxico */
int IsAddOp(char c);
int IsMulOp(char c);
int IsOrOp(char c);
int IsRelOp(char c);
void SkipWhite();
void NewLine();
void Match(char c);
char GetName();
char GetNum();


/* Programa principal */
int main()
{
    Init();
    DumpTable();

    return 0;
}

/* Inicialização do compilador */
void Init()
{
    int i;

    for (i = 0; i < SYMBOLTABLE_SIZE; i++)
        SymbolTable[i] = '?';

    NextChar();
    SkipWhite();
}

/* Lê próximo caracter da entrada em lookahead */
void NextChar()
{
    Look = getchar();
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

/* Avisa a respeito de uma palavra-chave desconhecida */
void Unrecognized(char name)
{
    Abort("Unrecognized keyword %c", name);
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

/* Exibe a tabela de símbolos */
void DumpTable()
{
    int i;

    printf("Symbol table dump:\n");

    for (i = 0; i < SYMBOLTABLE_SIZE; i++)
        printf("%c = %c\n", i + 'A', SymbolTable[i]);
}

/* Testa operadores de adição */
int IsAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* Testa operadores de multiplicação */
int IsMulOp(char c)
{
    return (c == '*' || c == '/');
}

/* Testa operadores OU */
int IsOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* Testa operadores relacionais */
int IsRelOp(char c)
{
    return (c == '=' || c == '#' || c == '<' || c == '>');
}

/* Pula caracteres em branco */
void SkipWhite()
{
    while (Look == ' ' || Look == '\t')
        NextChar();
}

/* Reconhece uma quebra de linha */
void NewLine()
{
    if (Look == '\n')
        NextChar();
}

/* Verifica se Look combina com caracter esperado */
void Match(char c)
{
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
    SkipWhite();
}

/* Analisa e traduz um nome (identificador ou palavra-chave) */
char GetName()
{
    char name;

    if (!isalpha(Look))
        Expected("Name");
    name = toupper(Look);
    NextChar();
    SkipWhite();

    return name;
}

/* Analisa e traduz um número inteiro */
char GetNum()
{
    char num;

    if (!isdigit(Look))
        Expected("Integer");
    num = Look;
    NextChar();
    SkipWhite();

    return num;
}
