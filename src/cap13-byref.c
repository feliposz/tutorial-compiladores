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

char Look; /* O caracter lido "antecipadamente" (lookahead) */

#define SYMBOLTABLE_SIZE 26
char SymbolTable[SYMBOLTABLE_SIZE]; /* Tabela de símbolos */

#define PARAMTABLE_SIZE 26
int ParamTable[PARAMTABLE_SIZE]; /* Lista de parâmetros formais para os procedimentos */
int ParamCount; /* Número de parâmetros formais */

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

/* Tratamento da tabela de parâmetros formais */
void ClearParams();
int ParamNumber(char name);
int IsParam(char name);
void AddParam(char name);

/* Analisador léxico */
int IsAddOp(char c);
int IsMulOp(char c);
void SkipWhite();
void NewLine();
void Match(char c);
char GetName();
char GetNum();

/* Geração de código */
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

/* Analisador sintático */
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

/* Programa principal */
int main()
{
    Init();
    AsmHeader();
    TopDeclarations();
    AsmEpilog();

    return 0;
}

/* Inicialização do compilador */
void Init()
{
    int i;

    for (i = 0; i < SYMBOLTABLE_SIZE; i++)
        SymbolTable[i] = ' ';

    ClearParams();

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
    if (IsParam(name))
        return 'f';
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

/* Limpa a tabela de parâmetros formais */
void ClearParams()
{
    int i;
    for (i = 0; i < PARAMTABLE_SIZE; i++)
        ParamTable[i] = 0;
    ParamCount = 0;
}

/* Retorna número indicando a posição do parâmetro */
int ParamNumber(char name)
{
    return ParamTable[name - 'A'];
}

/* Verifica se nome é parâmetro */
int IsParam(char name)
{
    return (ParamTable[name - 'A'] != 0);
}

/* Adiciona parâmetro à lista */
void AddParam(char name)
{
    if (IsParam(name))
        Duplicate(name);
    ParamTable[name - 'A'] = ++ParamCount;
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

/* Cabeçalho inicial para o montador */
void AsmHeader()
{
    printf("org 100h\n");
    printf("section .text\n");
    EmitLn("JMP _start");
}

/* Emite código para o prólogo de um programa */
void AsmProlog()
{
    printf("section .text\n");
    printf("_start:\n");
}

/* Emite código para o epílogo de um programa */
void AsmEpilog()
{
    EmitLn("MOV AX, 4C00h");
    EmitLn("INT 21h");
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
    printf("section .data\n");
    printf("%c\tdw 0\n", name);
}

/* Gera uma chamada de procedimento */
void AsmCall(char name)
{
    EmitLn("CALL %c", name);
}

/* Retorno de sub-rotina */
void AsmReturn()
{
    EmitLn("RET");
}

/* Calcula deslocamento do parâmetro na pilha */
int AsmOffsetParam(int par)
{
    int offset;

    /* Offset = (endereço de retorno + BP) + tamanho do parâmetro * posição relativa */
    offset = 4 + 2 * (ParamCount - par); 

    return offset;
}

/* Carrega parâmetro por referência em registrador primário */
void AsmLoadParam(int par)
{
    int offset = AsmOffsetParam(par);
    EmitLn("MOV BX, [BP+%d]", offset);
    EmitLn("MOV AX, [BX]");
}

/* Armazena conteúdo do registrador primário em parâmetro por referência */
void AsmStoreParam(int par)
{
    int offset = AsmOffsetParam(par);
    EmitLn("MOV BX, [BP+%d]", offset);
    EmitLn("MOV [BX], AX");
}

/* Coloca parâmetros na pilha */
void AsmPushParam(char name)
{
    switch (SymbolType(name)) {
        case 'v':
            EmitLn("MOV AX, %c", name);
            AsmPush();
            break;
        case 'f':
            EmitLn("MOV AX, [BP+%d]", AsmOffsetParam(ParamNumber(name)));
            AsmPush();
            break;
        default:
            Abort("Identifier %c cannot be used here!", name);
    }
}

/* Coloca registrador primário na pilha */
void AsmPush()
{
    EmitLn("PUSH AX");
}

/* Ajusta o ponteiro da pilha acima */
void AsmCleanStack(int bytes)
{
    if (bytes > 0)
        EmitLn("ADD SP, %d", bytes);
}

/* Escreve o prólogo para um procedimento */
void AsmProcProlog(char name)
{
    printf("section .text\n");
    printf("%c:\n", name);
    EmitLn("PUSH BP");
    EmitLn("MOV BP, SP");
}

/* Escreve o epílogo para um procedimento */
void AsmProcEpilog()
{
    EmitLn("POP BP");
    EmitLn("RET");
}

/* Analisa e traduz uma expressão */
void Expression()
{
    char name = GetName();
    if (IsParam(name))
        AsmLoadParam(ParamNumber(name));
    else
        AsmLoadVar(name);
}

/* Analisa e traduz um comando de atribuição */
void Assignment(char name)
{
    Match('=');
    Expression();
    if (IsParam(name))
        AsmStoreParam(ParamNumber(name));
    else
        AsmStoreVar(name);
}

/* Analisa e traduz um comando de atribuição ou chamada de procedimento */
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

/* Analiza e traduz um bloco de comandos */
void DoBlock()
{
    while (Look != 'e') {
        AssignOrProc();
        NewLine();
    }
}

/* Analiza e traduz um bloco begin */
void BeginBlock()
{
    Match('b');
    NewLine();
    DoBlock();
    Match('e');
    NewLine();
}

/* Processa um parâmetro de chamada */
void Param()
{
    char name = GetName();
    AsmPushParam(name);
}

/* Processa a lista de parâmetros para uma chamada de procedimento */
int ParamList()
{
    int count = 0;;

    Match('(');
    if (Look != ')') {
        for (;;) {
            Param();
            count++;
            if (Look != ',')
                break;
            Match(',');
        }
    }
    Match(')');

    return count * 2; /* Número de parâmetros * bytes por parâmetro */
}

/* Processa uma chamada de procedimento */
void CallProc(char name)
{
    int bytes = ParamList();
    AsmCall(name);
    AsmCleanStack(bytes);
}

/* Processa um parâmetro formal */
void FormalParam()
{
    char name;

    name = GetName();
    AddParam(name);
}

/* Processa a lista de parâmetros formais de um procedimento */
void FormalList()
{
    Match('(');
    if (Look != ')') {
        FormalParam();
        while (Look == ',') {
            Match(',');
            FormalParam();
        }
    }
    Match(')');
    NewLine();
}

/* Analisa e traduz uma declaração de procedimento */
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

/* Analiza e traduz uma declaração de variável */
void Declaration()
{
    Match('v');
    AsmAllocVar(GetName());
}

/* Analiza e traduz as declarações globais */
void TopDeclarations()
{
    while (Look != '.') {
        switch (Look) {
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
                Unrecognized(Look);
                break;
        }
        NewLine();
    }
}

/* Analiza e traduz o bloco principal do programa */
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