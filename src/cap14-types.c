/*
Tipos de variáveis

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
char SymbolType(char name);
char InTable(char name);
void Duplicate(char name);
void CheckDuplicate(char name);
void AddEntry(char name, char type);

/* Geração de código */
void AsmAllocVar(char name, char type);
void AsmLoadVar(char name, char type);
void AsmStoreVar(char name, char type);
void AsmConvert(char src, char dst);
void AsmLoadConst(long val, char type);
void AsmClear();
void AsmPush(char type);
void AsmPop(char type);
void AsmSwap(char type);
char AsmSameType(char t1, char t2, int ordMatters);
char AsmPopAdd(char t1, char t2);
char AsmPopSub(char t1, char t2);
char AsmPopMul(char t1, char t2);
char AsmPopDiv(char t1, char t2);

/* Analisador léxico */
int IsAddOp(char c);
int IsMulOp(char c);
int IsOrOp(char c);
int IsRelOp(char c);
int IsVarType(char c);
void SkipWhite();
void NewLine();
void Match(char c);
char GetName();
long GetNum();

/* Analisador sintático */
void AllocVar(char name, char type);
char LoadVar(char name);
char LoadNum(long val);
void StoreVar(char name, char type);
void NotVar(char name);
char VarType(char name);
void Declaration();
void TopDeclarations();
char UnaryOp();
char Factor();
char Multiply(char type);
char Divide(char type);
char Term();
char Add(char type);
char Subtract(char type);
char Expression();
void Assignment();
void Block();

/* Programa principal */
int main()
{
    Init();
    TopDeclarations();
    Match('B');
    NewLine();
    Block();
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
        if (SymbolTable[i] != '?')
            printf("%c = %c\n", i + 'A', SymbolTable[i]);
}

/* Retorna o tipo de um identificador */
char SymbolType(char name)
{
        return SymbolTable[name - 'A'];
}

/* Verifica se "name" consta na tabela de símbolos */
char InTable(char name)
{
        return (SymbolTable[name - 'A'] != '?');
}

/* Avisa a respeito de um identificador desconhecido */
void Duplicate(char name)
{
    fprintf(stderr, "Error: Duplicated identifier %c\n", name);
    exit(1);
}

/* Verifica se um identificador já foi declarado */
void CheckDuplicate(char name)
{
        if (InTable(name))
                Duplicate(name);
}

/* Adiciona nova entrada à tabela de símbolos */
void AddEntry(char name, char type)
{
    CheckDuplicate(name);
        SymbolTable[name - 'A'] = type;
}

/* Gera código para armazenamento de variável */
void AsmAllocVar(char name, char type)
{
    int btype; /* Tamanho em bytes */

    switch (type) {
        case 'b':
        case 'w':
            btype = type;
            break;
        case 'l':
            btype = 'd';
            break;
    }
    printf("%c d%c 0\n", name, btype);
}

/* Gera código para carregar variável de acordo com o tipo */
void AsmLoadVar(char name, char type)
{
    switch (type) {
        case 'b':
            EmitLn("MOV AL, [%c]", name);
            break;
        case 'w':
            EmitLn("MOV AX, [%c]", name);
            break;
        case 'l':
            EmitLn("MOV DX, [%c+2]", name);
            EmitLn("MOV AX, [%c]", name);
    }
}

/* Gera código para armazenar variável de acordo com o tipo */
void AsmStoreVar(char name, char type)
{
    switch (type) {
        case 'b':
            EmitLn("MOV [%c], AL", name);
            break;
        case 'w':
            EmitLn("MOV [%c], AX", name);
            break;
        case 'l':
            EmitLn("MOV [%c+2], DX", name);
            EmitLn("MOV [%c], AX", name);
    }
}

/* Converte tipo origem para destino */
void AsmConvert(char src, char dst)
{
    if (src == dst)
        return;
    if (src == 'b')
        EmitLn("CBW");
    if (dst == 'l')
        EmitLn("CWD");
}

/* Gera código para carregar uma constante de acordo com o tipo */
void AsmLoadConst(long val, char type)
{
    switch (type) {
        case 'b':
            EmitLn("MOV AL, %d", (int) val);
            break;
        case 'w':
            EmitLn("MOV AX, %d", (int) val);
            break;
        case 'l':
            EmitLn("MOV DX, %u", val >> 16);
            EmitLn("MOV AX, %u", val & 0xFFFF);
            break;
    }
}

/* Zera o registrador primário */
void AsmClear()
{
    EmitLn("XOR AX, AX");
}

/* Coloca valor na pilha */
void AsmPush(char type)
{
    if (type == 'b')
        EmitLn("CBW"); /* Só é possível empilhar "word"s */
    if (type == 'l')
        EmitLn("PUSH DX");
    EmitLn("PUSH AX");
}

/* Coloca em registrador(es) secundário(s) valor da pilha */
void AsmPop(char type)
{
    EmitLn("POP BX");
    if (type == 'l')
        EmitLn("POP CX");
}

/* Gera código para trocar registradores primário e secundário */
void AsmSwap(char type)
{
    switch (type) {
      case 'b':
          EmitLn("XCHG AL, BL");
          break;
      case 'w':
          EmitLn("XCHG AX, BX");
          break;
      case 'l':
          EmitLn("XCHG AX, BX");
          EmitLn("XCHG DX, CX");
          break;
    }
}

/* Faz a promoção dos tipos dos operandos e inverte a ordem dos mesmos se necessário */
char AsmSameType(char t1, char t2, int ordMatters)
{
    int swaped = 0;
    int type = t1;

    if (t1 != t2) {
        if ((t1 == 'b') || (t1 == 'w' && t2 == 'l')) {
            type = t2;
            AsmSwap(type);
            AsmConvert(t1, t2);
            swaped = 1;
        } else {
            type = t1;
            AsmConvert(t2, t1);
        }
    }
    if (!swaped && ordMatters)
        AsmSwap(type);
        
    return type;
}

/* Soma valor na pilha com valor no registrador primário */
char AsmPopAdd(char t1, char t2)
{
    char type;

    AsmPop(t1);
    type = AsmSameType(t1, t2, 0);
    switch (type) {
        case 'b':
            EmitLn("ADD AL, BL");
            break;
        case 'w':
            EmitLn("ADD AX, BX");
            break;
        case 'l':
            EmitLn("ADD AX, BX");
            EmitLn("ADC DX, CX");
            break;
    }

    return type;
}

/* Subtrai do valor da pilha o valor no registrador primário */
char AsmPopSub(char t1, char t2)
{
    char type;

    AsmPop(t1);
    type = AsmSameType(t1, t2, 1);
    switch (type) {
        case 'b':
            EmitLn("SUB AL, BL");
            break;
        case 'w':
            EmitLn("SUB AX, BX");
            break;
        case 'l':
            EmitLn("SUB AX, BX");
            EmitLn("SBB DX, CX");
            break;
    }

    return type;
}

/* Multiplica valor na pilha com valor no registrador primário */
char AsmPopMul(char t1, char t2)
{
    char type, mulType;

    AsmPop(t1);
    type = AsmSameType(t1, t2, 0);
    mulType = 'l';
    switch (type) {
        case 'b':
            EmitLn("IMUL BL");
            mulType = 'w';
            break;
        case 'w':
            EmitLn("IMUL BX");
            break;
        case 'l':
            EmitLn("CALL MUL32");
            break;
    }

    return mulType;
}

/* Divide valor na pilha por valor do registrador primário */
char AsmPopDiv(char t1, char t2)
{
    AsmPop(t1);

    /* Se dividendo for 32-bits divisor deve ser também */
    if (t1 == 'l')
        AsmConvert(t2, 'l');

    /* Coloca operandos na ordem certa conforme o tipo */
    if (t1 == 'l' || t2 == 'l')
        AsmSwap('l');
    else if (t1 == 'w' || t2 == 'w')
        AsmSwap('w');
    else
        AsmSwap('b');

    /* Dividendo _REAL_ sempre será LONG...
        mas WORD se divisor for BYTE */
    if (t2 == 'b')
        AsmConvert(t1, 'w');
    else
        AsmConvert(t1, 'l');

    /* Se um dos operandos for LONG, divisão de 32-bits */
    if (t1 == 'l' || t2 == 'l')
        EmitLn("CALL DIV32");
    else if (t2 == 'w') /* 32 / 16 */
        EmitLn("IDIV BX");
    else if (t2 == 'b') /* 16 / 8 */
        EmitLn("IDIV BL");

    /* Tipo do quociente é sempre igual ao do dividendo */
    return t1;
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

/* Reconhece um tipo de variável válido */
int IsVarType(char c)
{
    return (c == 'b' || c == 'w' || c == 'l');
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

/* Analisa e traduz um número inteiro longo */
long GetNum()
{
    long num;

    if (!isdigit(Look))
        Expected("Integer");
    num = 0;
    while (isdigit(Look)) {
        num *= 10;
        num += Look - '0';
        NextChar();
    }
    SkipWhite();
    return num;
}

/* Aloca espaço de armazenamento para variável */
void AllocVar(char name, char type)
{
    AddEntry(name, type);
    AsmAllocVar(name, type);
}

/* Carrega variável */
char LoadVar(char name)
{
    char type = VarType(name);
    AsmLoadVar(name, type);
    return type;
}

/* Carrega uma constante no registrador primário */
char LoadNum(long val)
{
    char type;

    if (val >= -128 && val <= 127)
        type = 'b';
    else if (val >= -32768 && val <= 32767)
        type = 'w';
    else
        type = 'l';
    AsmLoadConst(val, type);

    return type;
}

/* Armazena variável */
void StoreVar(char name, char srcType)
{
    char dstType = VarType(name);
    AsmConvert(srcType, dstType);
    AsmStoreVar(name, dstType);
}

/* Avisa a respeito de um identificador que não é uma variável */
void NotVar(char name)
{
    Abort("'%c' is not a variable", name);
}

/* Pega o tipo da variável da tabela de símbolos */
char VarType(char name)
{
    char type;

    type = SymbolType(name);
    if (!IsVarType(type))
        NotVar(name);

    return type;
}

/* Analiza e traduz uma declaração de variável */
void Declaration()
{
    char type = Look;
    NextChar();
    AllocVar(GetName(), type);
}

/* Analiza e traduz as declarações globais */
void TopDeclarations()
{
    while (Look != 'B') {
        switch (Look) {
            case 'b':
            case 'w':
            case 'l':
                Declaration();
                break;
            default:
                Unrecognized(Look);
                break;
        }
        NewLine();
    }
}

/* Tratamento de operador unário */
char UnaryOp()
{
    AsmClear();
    return 'w';
}

/* Analisa e traduz um fator matemático */
char Factor()
{
    char type;

    if (Look == '(') {
        Match('(');
        type = Expression();
        Match(')');
    } else if (isalpha(Look))
        type = LoadVar(GetName());
    else
        type = LoadNum(GetNum());

    return type;
}

/* Reconhece e traduz uma multiplicação */
char Multiply(char type)
{
    Match('*');
    return AsmPopMul(type, Factor());
}

/* Reconhece e traduz uma multiplicação */
char Divide(char type)
{
    Match('/');
    return AsmPopDiv(type, Factor());
}

/* Analisa e traduz um termo matemático */
char Term()
{
    char type;

    type = Factor();
    while (IsMulOp(Look)) {
        AsmPush(type);
        switch (Look) {
            case '*':
                type = Multiply(type);
                break;
            case '/':
                type = Divide(type);
                break;
        }
    }

    return type;
}

/* Reconhece e traduz uma soma */
char Add(char type)
{
    Match('+');
    return AsmPopAdd(type, Term());
}

/* Reconhece e traduz uma subtração */
char Subtract(char type)
{
    Match('-');
    return AsmPopSub(type, Term());
}

/* Analisa e traduz uma expressão */
char Expression()
{
    char type;

    if (IsAddOp(Look))
        type = UnaryOp();
    else
        type = Term();
    while (IsAddOp(Look)) {
        AsmPush(type);
        switch (Look) {
            case '+':
                type = Add(type);
                break;
            case '-':
                type = Subtract(type);
                break;
        }
    }

    return type;
}

/* Analisa e traduz uma atribuição */
void Assignment()
{
    char name, type;

    name = GetName();
    Match('=');
    type = Expression();
    StoreVar(name, type);
}

/* Analisa e traduz um bloco de comandos */
void Block()
{
    while (Look != '.') {
        Assignment();
        NewLine();
    }
}