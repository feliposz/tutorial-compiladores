/*
Procedimentos - Base para experiências

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

char Look; /* O caracter lido "antecipadamente" (lookahead) */

#define SYMBOLTABLE_SIZE 26
char SymbolTable[SYMBOLTABLE_SIZE]; /* tabela de símbolos */

/* rotinas utilitárias */
void Init();
void NextChar();
void Error(char *fmt, ...);
void Abort(char *fmt, ...);
void Expected(char *fmt, ...);
void EmitLn(char *fmt, ...);
void Undefined(char name);
void Duplicate(char name);
void Unrecognized(char name);
void NotVar(char name);

/* tratamento da tabela de símbolos */
char SymbolType(char name);
char InTable(char name);
void AddEntry(char name, char type);
void CheckVar(char name);

/* analisador léxico rudimentar */
int IsAddOp(char c);
int IsMulOp(char c);
void SkipWhite();
void NewLine();
void Match(char c);
char GetName();
char GetNum();

/* geração de código */
void AsmLoadVar(char name);
void AsmStoreVar(char name);
void AsmAllocVar(char name);

/* analisador sintático */
void Expression();
void Assignment();
void DoBlock();
void BeginBlock();
void Declaration();
void TopDeclarations();

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    TopDeclarations();
    BeginBlock();

    return 0;
}

/* inicialização do compilador */
void Init()
{
    int i;

    for (i = 0; i < SYMBOLTABLE_SIZE; i++)
        SymbolTable[i] = ' ';

    NextChar();
    SkipWhite();
}

/* lê próximo caracter da entrada em lookahead */
void NextChar()
{
    Look = getchar();
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

/* avisa a respeito de um identificador desconhecido */
void Undefined(char name)
{
    Abort("Undefined identifier %c", name);
}

/* avisa a respeito de um identificador desconhecido */
void Duplicate(char name)
{
    Abort("Duplicated identifier %c", name);
}

/* avisa a respeito de uma palavra-chave desconhecida */
void Unrecognized(char name)
{
    Abort("Unrecognized keyword %c", name);
}

/* avisa a respeito de um identificador que não é variável */
void NotVar(char name)
{
    Abort("'%c' is not a variable", name);
}

/* retorna o tipo de um identificador */
char SymbolType(char name)
{
    return SymbolTable[name - 'A'];
}

/* verifica se "name" consta na tabela de símbolos */
char InTable(char name)
{
    return (SymbolTable[name - 'A'] != ' ');
}

/* adiciona novo identificador à tabela de símbolos */
void AddEntry(char name, char type)
{
    if (InTable(name))
        Duplicate(name);
    SymbolTable[name - 'A'] = type;
}

/* verifica se identificador é variável */
void CheckVar(char name)
{
    if (!InTable(name))
        Undefined(name);
    if (SymbolType(name) != 'v')
        NotVar(name);
}

/* testa operadores de adição */
int IsAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* testa operadores de multiplicação */
int IsMulOp(char c)
{
    return (c == '*' || c == '/');
}

/* pula caracteres em branco */
void SkipWhite()
{
    while (Look == ' ' || Look == '\t')
        NextChar();
}

/* reconhece uma quebra de linha */
void NewLine()
{
    if (Look == '\n')
        NextChar();
}

/* verifica se Look combina com caracter esperado */
void Match(char c)
{
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
    SkipWhite();
}

/* analisa e traduz um nome (identificador ou palavra-chave) */
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

/* analisa e traduz um número inteiro */
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

/* carrega uma variável no registrador primário */
void AsmLoadVar(char name)
{
    CheckVar(name);
    EmitLn("MOV AX, WORD PTR %c", name);
}

/* armazena registrador primário em variável */
void AsmStoreVar(char name)
{
    EmitLn("MOV WORD PTR %c, AX", name);
}

/* aloca espaço de armazenamento para variável */
void AsmAllocVar(char name)
{
    if (InTable(name))
        Duplicate(name);
    AddEntry(name, 'v');
    printf("%c\tdw 0\n", name);
}

/* analisa e traduz uma expressão */
void Expression()
{
    AsmLoadVar(GetName());
}

/* analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    Expression();
    AsmStoreVar(name);
}

/* analiza e traduz um bloco de comandos */
void DoBlock()
{
    while (Look != 'e') {
        Assignment();
        NewLine();
    }
}

/* analiza e traduz um bloco begin */
void BeginBlock()
{
    Match('b');
    NewLine();
    DoBlock();
    Match('e');
    NewLine();
}

/* analiza e traduz a declaração de uma variável */
void Declaration()
{
    Match('v');
    AsmAllocVar(GetName());
}

/* analiza e traduz as declarações globais */
void TopDeclarations()
{
    while (Look != 'b') {
        switch (Look) {
            case 'v':
                Declaration();
                break;
            default:
                Unrecognized(Look);
                break;
        }
        NewLine();
    }
}