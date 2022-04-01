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

char Look; /* O caractere lido "antecipadamente" (lookahead) */

#define SYMBOLTABLE_SIZE 26
char SymbolTable[SYMBOLTABLE_SIZE]; /* Tabela de símbolos */

/* Rotinas utilitárias */
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

/* Tratamento da tabela de símbolos */
char SymbolType(char name);
char InTable(char name);
void AddEntry(char name, char type);
void CheckVar(char name);

/* Analisador léxico */
int IsAddOp(char c);
int IsMulOp(char c);
void SkipWhite();
void NewLine();
void Match(char c);
char GetName();
char GetNum();

/* Geração de código */
void AsmLoadVar(char name);
void AsmStoreVar(char name);
void AsmAllocVar(char name);

/* Analisador sintático */
void Expression();
void Assignment();
void DoBlock();
void BeginBlock();
void Declaration();
void TopDeclarations();

/* Programa principal */
int main()
{
    Init();
    TopDeclarations();
    BeginBlock();

    return 0;
}

/* Inicialização do compilador */
void Init()
{
    int i;

    for (i = 0; i < SYMBOLTABLE_SIZE; i++)
        SymbolTable[i] = ' ';

    NextChar();
    SkipWhite();
}

/* Lê próximo caractere da entrada em lookahead */
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

/* Avisa a respeito de um identificador desconhecido */
void Undefined(char name)
{
    Abort("Undefined identifier %c", name);
}

/* Avisa a respeito de um identificador desconhecido */
void Duplicate(char name)
{
    Abort("Duplicated identifier %c", name);
}

/* Avisa a respeito de uma palavra-chave desconhecida */
void Unrecognized(char name)
{
    Abort("Unrecognized keyword %c", name);
}

/* Avisa a respeito de um identificador que não é variável */
void NotVar(char name)
{
    Abort("'%c' is not a variable", name);
}

/* Retorna o tipo de um identificador */
char SymbolType(char name)
{
    return SymbolTable[name - 'A'];
}

/* Verifica se "name" consta na tabela de símbolos */
char InTable(char name)
{
    return (SymbolTable[name - 'A'] != ' ');
}

/* Adiciona nova entrada à tabela de símbolos */
void AddEntry(char name, char type)
{
    if (InTable(name))
        Duplicate(name);
    SymbolTable[name - 'A'] = type;
}

/* Verifica se identificador é variável */
void CheckVar(char name)
{
    if (!InTable(name))
        Undefined(name);
    if (SymbolType(name) != 'v')
        NotVar(name);
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

/* Verifica se Look combina com caractere esperado */
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

/* Carrega uma variável no registrador primário */
void AsmLoadVar(char name)
{
    CheckVar(name);
    EmitLn("MOV AX, [%c]", name);
}

/* Armazena registrador primário em variável */
void AsmStoreVar(char name)
{
    EmitLn("MOV [%c], AX", name);
}

/* Aloca espaço de armazenamento para variável */
void AsmAllocVar(char name)
{
    if (InTable(name))
        Duplicate(name);
    AddEntry(name, 'v');
    printf("%c\tdw 0\n", name);
}

/* Analisa e traduz uma expressão */
void Expression()
{
    AsmLoadVar(GetName());
}

/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    Expression();
    AsmStoreVar(name);
}

/* Analisa e traduz um bloco de comandos */
void DoBlock()
{
    while (Look != 'e') {
        Assignment();
        NewLine();
    }
}

/* Analisa e traduz um bloco begin */
void BeginBlock()
{
    Match('b');
    NewLine();
    DoBlock();
    Match('e');
    NewLine();
}

/* Analisa e traduz uma declaração de variável */
void Declaration()
{
    Match('v');
    AsmAllocVar(GetName());
}

/* Analisa e traduz as declarações globais */
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