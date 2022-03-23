/*
Procedimentos - Passando parâmetros por referência

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

char look; /* O caracter lido "antecipadamente" (lookahead) */

#define SYMBOLTABLE_SIZE 26
char SymbolTable[SYMBOLTABLE_SIZE]; /* tabela de símbolos */

#define PARAMTABLE_SIZE 26
int ParamTable[PARAMTABLE_SIZE]; /* lista de parâmetros formais para os procedimentos */
int ParamCount; /* número de parâmetros formais */

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

/* tratamento da tabela de parâmetros formais */
void ClearParams();
int ParamNumber(char name);
int IsParam(char name);
void AddParam(char name);

/* analisador léxico rudimentar */
int IsAddOp(char c);
int IsMulOp(char c);
void SkipWhite();
void NewLine();
void Match(char c);
char GetName();
char GetNum();

/* geração de código */
void AsmHeader();
void AsmProlog();
void AsmEpilog();
void AsmLoadVar(char name);
void AsmStoreVar(char name);
void AsmAllocVar(char name);
void AsmCall(char name);
void AsmReturn();
int AsmOffsetParam(int par);
void AsmLoadParam(int par);
void AsmStoreParam(int par);
void AsmPushParam(char name);
void AsmPush();
void AsmCleanStack(int bytes);
void AsmProcProlog(char name);
void AsmProcEpilog();

/* analisador sintático */
void Expression();
void Assignment(char name);
void AssignOrProc();
void DoBlock();
void BeginBlock();
void Param();
int ParamList();
void CallProc(char name);
void FormalList();
void FormalParam();
void DoProcedure();
void Declaration();
void TopDeclarations();
void DoMain();

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    AsmHeader();
    TopDeclarations();
    AsmEpilog();

    return 0;
}

/* inicialização do compilador */
void Init()
{
    int i;

    for (i = 0; i < SYMBOLTABLE_SIZE; i++)
        SymbolTable[i] = ' ';

    ClearParams();

    NextChar();
    SkipWhite();
}

/* lê próximo caracter da entrada em lookahead */
void NextChar()
{
    look = getchar();
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
    if (IsParam(name))
        return 'f';
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

/* limpa a tabela de parâmetros formais */
void ClearParams()
{
    int i;
    for (i = 0; i < PARAMTABLE_SIZE; i++)
        ParamTable[i] = 0;
    ParamCount = 0;
}

/* retorna número indicando a posição do parâmetro */
int ParamNumber(char name)
{
    return ParamTable[name - 'A'];
}

/* verifica se nome é parâmetro */
int IsParam(char name)
{
    return (ParamTable[name - 'A'] != 0);
}

/* adiciona parâmetro à lista */
void AddParam(char name)
{
    if (IsParam(name))
        Duplicate(name);
    ParamTable[name - 'A'] = ++ParamCount;
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
    while (look == ' ' || look == '\t')
        NextChar();
}

/* reconhece uma quebra de linha */
void NewLine()
{
    if (look == '\n')
        NextChar();
}

/* verifica se look combina com caracter esperado */
void Match(char c)
{
    if (look != c)
        Expected("'%c'", c);
    NextChar();
    SkipWhite();
}

/* analisa e traduz um nome (identificador ou palavra-chave) */
char GetName()
{
    char name;

    if (!isalpha(look))
        Expected("Name");
    name = toupper(look);
    NextChar();
    SkipWhite();

    return name;
}

/* analisa e traduz um número inteiro */
char GetNum()
{
    char num;

    if (!isdigit(look))
        Expected("Integer");
    num = look;
    NextChar();
    SkipWhite();

    return num;
}

/* cabeçalho inicial para o montador */
void AsmHeader()
{
    EmitLn(".model small");
    EmitLn(".stack");
    EmitLn(".code");
    printf("PROG segment byte public\n");
    EmitLn("assume cs:PROG,ds:PROG,es:PROG,ss:PROG");
}

/* emite código para o prólogo de um programa */
void AsmProlog()
{
    printf("MAIN:\n");
    EmitLn("MOV AX, PROG");
    EmitLn("MOV DS, AX");
    EmitLn("MOV ES, AX");
}

/* emite código para o epílogo de um programa */
void AsmEpilog()
{
    EmitLn("MOV AX, 4C00h");
    EmitLn("INT 21h");
    printf("PROG ends\n");
    EmitLn("end MAIN");
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

/* gera uma chamada de procedimento */
void AsmCall(char name)
{
    EmitLn("CALL %c", name);
}

/* retorno de sub-rotina */
void AsmReturn()
{
    EmitLn("RET");
}

/* calcula deslocamento do parâmetro na pilha */
int AsmOffsetParam(int par)
{
    int offset;

    /* offset = (endereço de retorno + BP) + tamanho do parâmetro * posição relativa */
    offset = 4 + 2 * (ParamCount - par); 

    return offset;
}

/* carrega parâmetro por referência em registrador primário */
void AsmLoadParam(int par)
{
    int offset = AsmOffsetParam(par);
    EmitLn("MOV BX, WORD PTR [BP+%d]", offset);
    EmitLn("MOV AX, WORD PTR [BX]");
}

/* armazena conteúdo do registrador primário em parâmetro por referência */
void AsmStoreParam(int par)
{
    int offset = AsmOffsetParam(par);
    EmitLn("MOV BX, WORD PTR [BP+%d]", offset);
    EmitLn("MOV WORD PTR [BX], AX");
}

/* coloca parâmetros na pilha */
void AsmPushParam(char name)
{
    switch (SymbolType(name)) {
        case 'v':
            EmitLn("MOV AX, OFFSET %c", name);
            AsmPush();
            break;
        case 'f':
            EmitLn("MOV AX, WORD PTR [BP+%d]", AsmOffsetParam(ParamNumber(name)));
            AsmPush();
            break;
        default:
            Abort("Identifier %c cannot be used here!", name);
    }
}

/* coloca registrador primário na pilha */
void AsmPush()
{
    EmitLn("PUSH AX");
}

/* ajusta o ponteiro da pilha acima */
void AsmCleanStack(int bytes)
{
    if (bytes > 0)
        EmitLn("ADD SP, %d", bytes);
}

/* escreve o prólogo para um procedimento */
void AsmProcProlog(char name)
{
    printf("%c:\n", name);
    EmitLn("PUSH BP");
    EmitLn("MOV BP, SP");
}

/* escreve o epílogo para um procedimento */
void AsmProcEpilog()
{
    EmitLn("POP BP");
    EmitLn("RET");
}

/* analisa e traduz uma expressão */
void Expression()
{
    char name = GetName();
    if (IsParam(name))
        AsmLoadParam(ParamNumber(name));
    else
        AsmLoadVar(name);
}

/* analisa e traduz um comando de atribuição */
void Assignment(char name)
{
    Match('=');
    Expression();
    if (IsParam(name))
        AsmStoreParam(ParamNumber(name));
    else
        AsmStoreVar(name);
}

/* analisa e traduz um comando de atribuição ou chamada de procedimento */
void AssignOrProc()
{
    char name;

    name = GetName();
    switch (SymbolType(name)) {
        case ' ':
            Undefined(name);
            break;
        case 'v':
        case 'f':
            Assignment(name);
            break;
        case 'p':
            CallProc(name);
            break;
        default:
            Abort("Identifier %c cannot be used here!", name);
    }
}

/* analiza e traduz um bloco de comandos */
void DoBlock()
{
    while (look != 'e') {
        AssignOrProc();
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

/* processa um parâmetro de chamada */
void Param()
{
    char name = GetName();
    AsmPushParam(name);
}

/* processa a lista de parâmetros para uma chamada de procedimento */
int ParamList()
{
    int count = 0;;

    Match('(');
    if (look != ')') {
        for (;;) {
            Param();
            count++;
            if (look != ',')
                break;
            Match(',');
        }
    }
    Match(')');

    return count * 2; /* número de parâmetros * bytes por parâmetro */
}

/* processa uma chamada de procedimento */
void CallProc(char name)
{
    int bytes = ParamList();
    AsmCall(name);
    AsmCleanStack(bytes);
}

/* processa um parâmetro formal */
void FormalParam()
{
    char name;

    name = GetName();
    AddParam(name);
}

/* processa a lista de parâmetros formais de um procedimento */
void FormalList()
{
    Match('(');
    if (look != ')') {
        FormalParam();
        while (look == ',') {
            Match(',');
            FormalParam();
        }
    }
    Match(')');
    NewLine();
}

/* analisa e traduz uma declaração de procedimento */
void DoProcedure()
{
    char name;

    Match('p');
    name = GetName();
    AddEntry(name, 'p');
    FormalList();
    AsmProcProlog(name);
    BeginBlock();
    AsmProcEpilog();
    ClearParams();
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
    while (look != '.') {
        switch (look) {
            case 'v':
                Declaration();
                break;
            case 'p':
                DoProcedure();
                break;
            case 'P':
                DoMain();
                break;
            default:
                Unrecognized(look);
                break;
        }
        NewLine();
    }
}

/* analiza e traduz o bloco principal do programa */
void DoMain()
{
    char name;

    Match('P');
    name = GetName();
    NewLine();
    if (InTable(name))
        Duplicate(name);
    AsmProlog();
    BeginBlock();
}