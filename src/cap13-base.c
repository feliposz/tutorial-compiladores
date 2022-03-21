/*
Procedimentos - Base para experiências

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack E. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
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
void emit(char *fmt, ...);
void undefined(char name);
void duplicated(char name);
void unrecognized(char name);
void notVar(char name);

/* tratamento da tabela de símbolos */
char symbolType(char name);
char inTable(char name);
void addSymbol(char name, char type);
void checkVar(char name);

/* analisador léxico rudimentar */
int isAddOp(char c);
int isMulOp(char c);
void skipWhite();
void newLine();
void match(char c);
char getName();
char getNum();

/* geração de código */
void asmLoadVar(char name);
void asmStoreVar(char name);
void asmAllocVar(char name);

/* analisador sintático */
void expression();
void assignment();
void doBlock();
void beginBlock();
void declaration();
void topDeclarations();

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    topDeclarations();
    beginBlock();

    return 0;
}

/* inicialização do compilador */
void init()
{
    int i;

    for (i = 0; i < SYMTBL_SZ; i++)
        symbolTable[i] = ' ';

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

/* avisa a respeito de um identificador desconhecido */
void undefined(char name)
{
    fatal("Undefined identifier %c", name);
}

/* avisa a respeito de um identificador desconhecido */
void duplicated(char name)
{
    fatal("Duplicated identifier %c", name);
}

/* avisa a respeito de uma palavra-chave desconhecida */
void unrecognized(char name)
{
    fatal("Unrecognized keyword %c", name);
}

/* avisa a respeito de um identificador que não é variável */
void notVar(char name)
{
    fatal("'%c' is not a variable", name);
}

/* retorna o tipo de um identificador */
char symbolType(char name)
{
    return symbolTable[name - 'A'];
}

/* verifica se "name" consta na tabela de símbolos */
char inTable(char name)
{
    return (symbolTable[name - 'A'] != ' ');
}

/* adiciona novo identificador à tabela de símbolos */
void addSymbol(char name, char type)
{
    if (inTable(name))
        duplicated(name);
    symbolTable[name - 'A'] = type;
}

/* verifica se identificador é variável */
void checkVar(char name)
{
    if (!inTable(name))
        undefined(name);
    if (symbolType(name) != 'v')
        notVar(name);
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

/* carrega uma variável no registrador primário */
void asmLoadVar(char name)
{
    checkVar(name);
    emit("MOV AX, WORD PTR %c", name);
}

/* armazena registrador primário em variável */
void asmStoreVar(char name)
{
    emit("MOV WORD PTR %c, AX", name);
}

/* aloca espaço de armazenamento para variável */
void asmAllocVar(char name)
{
    if (inTable(name))
        duplicated(name);
    addSymbol(name, 'v');
    printf("%c\tdw 0\n", name);
}

/* analisa e traduz uma expressão */
void expression()
{
    asmLoadVar(getName());
}

/* analisa e traduz um comando de atribuição */
void assignment()
{
    char name;

    name = getName();
    match('=');
    expression();
    asmStoreVar(name);
}

/* analiza e traduz um bloco de comandos */
void doBlock()
{
    while (look != 'e') {
        assignment();
        newLine();
    }
}

/* analiza e traduz um bloco begin */
void beginBlock()
{
    match('b');
    newLine();
    doBlock();
    match('e');
    newLine();
}

/* analiza e traduz a declaração de uma variável */
void declaration()
{
    match('v');
    asmAllocVar(getName());
}

/* analiza e traduz as declarações globais */
void topDeclarations()
{
    while (look != 'b') {
        switch (look) {
            case 'v':
                declaration();
                break;
            default:
                unrecognized(look);
                break;
        }
        newLine();
    }
}