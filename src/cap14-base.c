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

char look; /* O caracter lido "antecipadamente" (lookahead) */

#define SYMTBL_SZ 26
char symbolTable[SYMTBL_SZ]; /* tabela de símbolos */

/* rotinas utilitárias */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void unrecognized(char name);
void emit(char *fmt, ...);

/* tratamento da tabela de símbolos */
void dumpTable();

/* analisador léxico rudimentar */
int isAddOp(char c);
int isMulOp(char c);
int isOrOp(char c);
int isRelOp(char c);
void skipWhite();
void newLine();
void match(char c);
char getName();
char getNum();


/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    dumpTable();

    return 0;
}

/* inicialização do compilador */
void init()
{
    int i;

    for (i = 0; i < SYMTBL_SZ; i++)
        symbolTable[i] = '?';

    nextChar();
    skipWhite();
}

/* lê próximo caracter da entrada em lookahead */
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

/* avisa a respeito de uma palavra-chave desconhecida */
void unrecognized(char name)
{
    fatal("Unrecognized keyword %c", name);
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

/* exibe a tabela de símbolos */
void dumpTable()
{
    int i;

    printf("Symbol table dump:\n");

    for (i = 0; i < SYMTBL_SZ; i++)
        printf("%c = %c\n", i + 'A', symbolTable[i]);
}

/* testa operadores de adição */
int isAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* testa operadores de multiplicação */
int isMulOp(char c)
{
    return (c == '*' || c == '/');
}

/* testa operadores OU */
int isOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* testa operadores relacionais */
int isRelOp(char c)
{
    return (c == '=' || c == '#' || c == '<' || c == '>');
}

/* pula caracteres em branco */
void skipWhite()
{
    while (look == ' ' || look == '\t')
        nextChar();
}

/* reconhece uma quebra de linha */
void newLine()
{
    if (look == '\n')
        nextChar();
}

/* verifica se look combina com caracter esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();
    skipWhite();
}

/* analisa e traduz um nome (identificador ou palavra-chave) */
char getName()
{
    char name;

    if (!isalpha(look))
        expected("Name");
    name = toupper(look);
    nextChar();
    skipWhite();

    return name;
}

/* analisa e traduz um número inteiro */
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
