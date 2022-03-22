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
char symbolType(char name);
char inTable(char name);
void duplicated(char name);
void checkDuplicated(char name);
void addSymbol(char name, char type);

/* geração de código */
void asmAllocVar(char name, char type);
void asmLoadVar(char name, char type);
void asmStoreVar(char name, char type);
void asmConvert(char src, char dst);
void asmLoadConst(long val, char type);
void asmClear();
void asmPush(char type);
void asmPop(char type);
void asmSwap(char type);
char asmSameType(char t1, char t2, int ordMatters);
char asmPopAdd(char t1, char t2);
char asmPopSub(char t1, char t2);
char asmPopMul(char t1, char t2);
char asmPopDiv(char t1, char t2);

/* analisador léxico rudimentar */
int isAddOp(char c);
int isMulOp(char c);
int isOrOp(char c);
int isRelOp(char c);
int isVarType(char c);
void skipWhite();
void newLine();
void match(char c);
char getName();
long getNum();

/* analisador sintático */
void allocVar(char name, char type);
char loadVar(char name);
char loadNum(long val);
void storeVar(char name, char type);
void notVar(char name);
char varType(char name);
void declaration();
void topDeclarations();
char unaryOp();
char factor();
char multiply(char type);
char divide(char type);
char term();
char add(char type);
char subtract(char type);
char expression();
void assignment();
void block();

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    topDeclarations();
    match('B');
    newLine();
    block();
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
        if (symbolTable[i] != '?')
            printf("%c = %c\n", i + 'A', symbolTable[i]);
}

/* retorna o tipo de um identificador */
char symbolType(char name)
{
        return symbolTable[name - 'A'];
}

/* verifica se "name" consta na tabela de símbolos */
char inTable(char name)
{
        return (symbolTable[name - 'A'] != '?');
}

/* avisa a respeito de um identificador desconhecido */
void duplicated(char name)
{
    fprintf(stderr, "Error: Duplicated identifier %c\n", name);
    exit(1);
}

/* verifica se um identificador já foi declarado */
void checkDuplicated(char name)
{
        if (inTable(name))
                duplicated(name);
}

/* adiciona novo identificador à tabela de símbolos */
void addSymbol(char name, char type)
{
    checkDuplicated(name);
        symbolTable[name - 'A'] = type;
}

/* gera código para armazenamento de variável */
void asmAllocVar(char name, char type)
{
    int btype; /* tamanho em bytes */

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

/* gera código para carregar variável de acordo com o tipo */
void asmLoadVar(char name, char type)
{
    switch (type) {
        case 'b':
            emit("MOV AL, BYTE PTR %c", name);
            break;
        case 'w':
            emit("MOV AX, WORD PTR %c", name);
            break;
        case 'l':
            emit("MOV DX, WORD PTR [%c+2]", name);
            emit("MOV AX, WORD PTR [%c]", name);
    }
}

/* gera código para armazenar variável de acordo com o tipo */
void asmStoreVar(char name, char type)
{
    switch (type) {
        case 'b':
            emit("MOV BYTE PTR %c, AL", name);
            break;
        case 'w':
            emit("MOV WORD PTR %c, AX", name);
            break;
        case 'l':
            emit("MOV WORD PTR [%c+2], DX", name);
            emit("MOV WORD PTR [%c], AX", name);
    }
}

/* converte tipo origem para destino */
void asmConvert(char src, char dst)
{
    if (src == dst)
        return;
    if (src == 'b')
        emit("CBW");
    if (dst == 'l')
        emit("CWD");
}

/* gera código para carregar uma constante de acordo com o tipo */
void asmLoadConst(long val, char type)
{
    switch (type) {
        case 'b':
            emit("MOV AL, %d", (int) val);
            break;
        case 'w':
            emit("MOV AX, %d", (int) val);
            break;
        case 'l':
            emit("MOV DX, %u", val >> 16);
            emit("MOV AX, %u", val & 0xFFFF);
            break;
    }
}

/* zera o registrador primário */
void asmClear()
{
    emit("XOR AX, AX");
}

/* coloca valor na pilha */
void asmPush(char type)
{
    if (type == 'b')
        emit("CBW"); /* só é possível empilhar "word"s */
    if (type == 'l')
        emit("PUSH DX");
    emit("PUSH AX");
}

/* coloca em registrador(es) secundário(s) valor da pilha */
void asmPop(char type)
{
    emit("POP BX");
    if (type == 'l')
        emit("POP CX");
}

/* gera código para trocar registradores primário e secundário */
void asmSwap(char type)
{
    switch (type) {
      case 'b':
          emit("XCHG AL, BL");
          break;
      case 'w':
          emit("XCHG AX, BX");
          break;
      case 'l':
          emit("XCHG AX, BX");
          emit("XCHG DX, CX");
          break;
    }
}

/* faz a promoção dos tipos dos operandos e inverte a ordem dos mesmos se necessário */
char asmSameType(char t1, char t2, int ordMatters)
{
    int swaped = 0;
    int type = t1;

    if (t1 != t2) {
        if ((t1 == 'b') || (t1 == 'w' && t2 == 'l')) {
            type = t2;
            asmSwap(type);
            asmConvert(t1, t2);
            swaped = 1;
        } else {
            type = t1;
            asmConvert(t2, t1);
        }
    }
    if (!swaped && ordMatters)
        asmSwap(type);
        
    return type;
}

/* soma valor na pilha com valor no registrador primário */
char asmPopAdd(char t1, char t2)
{
    char type;

    asmPop(t1);
    type = asmSameType(t1, t2, 0);
    switch (type) {
        case 'b':
            emit("ADD AL, BL");
            break;
        case 'w':
            emit("ADD AX, BX");
            break;
        case 'l':
            emit("ADD AX, BX");
            emit("ADC DX, CX");
            break;
    }

    return type;
}

/* subtrai do valor da pilha o valor no registrador primário */
char asmPopSub(char t1, char t2)
{
    char type;

    asmPop(t1);
    type = asmSameType(t1, t2, 1);
    switch (type) {
        case 'b':
            emit("SUB AL, BL");
            break;
        case 'w':
            emit("SUB AX, BX");
            break;
        case 'l':
            emit("SUB AX, BX");
            emit("SBB DX, CX");
            break;
    }

    return type;
}

/* multiplica valor na pilha com valor no registrador primário */
char asmPopMul(char t1, char t2)
{
    char type, mulType;

    asmPop(t1);
    type = asmSameType(t1, t2, 0);
    mulType = 'l';
    switch (type) {
        case 'b':
            emit("IMUL BL");
            mulType = 'w';
            break;
        case 'w':
            emit("IMUL BX");
            break;
        case 'l':
            emit("CALL MUL32");
            break;
    }

    return mulType;
}

/* divide valor na pilha por valor do registrador primário */
char asmPopDiv(char t1, char t2)
{
    asmPop(t1);

    /* se dividendo for 32-bits divisor deve ser também */
    if (t1 == 'l')
        asmConvert(t2, 'l');

    /* coloca operandos na ordem certa conforme o tipo */
    if (t1 == 'l' || t2 == 'l')
        asmSwap('l');
    else if (t1 == 'w' || t2 == 'w')
        asmSwap('w');
    else
        asmSwap('b');

    /* dividendo _REAL_ sempre será LONG...
        mas WORD se divisor for BYTE */
    if (t2 == 'b')
        asmConvert(t1, 'w');
    else
        asmConvert(t1, 'l');

    /* se um dos operandos for LONG, divisão de 32-bits */
    if (t1 == 'l' || t2 == 'l')
        emit("CALL DIV32");
    else if (t2 == 'w') /* 32 / 16 */
        emit("IDIV BX");
    else if (t2 == 'b') /* 16 / 8 */
        emit("IDIV BL");

    /* tipo do quociente é sempre igual ao do dividendo */
    return t1;
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

/* reconhece um tipo de variável válido */
int isVarType(char c)
{
    return (c == 'b' || c == 'w' || c == 'l');
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

/* analisa e traduz um número inteiro longo */
long getNum()
{
    long num;

    if (!isdigit(look))
        expected("Integer");
    num = 0;
    while (isdigit(look)) {
        num *= 10;
        num += look - '0';
        nextChar();
    }
    skipWhite();
    return num;
}

/* aloca espaço de armazenamento para variável */
void allocVar(char name, char type)
{
    addSymbol(name, type);
    asmAllocVar(name, type);
}

/* carrega variável */
char loadVar(char name)
{
    char type = varType(name);
    asmLoadVar(name, type);
    return type;
}

/* carrega uma constante no registrador primário */
char loadNum(long val)
{
    char type;

    if (val >= -128 && val <= 127)
        type = 'b';
    else if (val >= -32768 && val <= 32767)
        type = 'w';
    else
        type = 'l';
    asmLoadConst(val, type);

    return type;
}

/* armazena variável */
void storeVar(char name, char srcType)
{
    char dstType = varType(name);
    asmConvert(srcType, dstType);
    asmStoreVar(name, dstType);
}

/* avisa a respeito de um identificador que não é uma variável */
void notVar(char name)
{
    fatal("'%c' is not a variable", name);
}

/* pega o tipo da variável da tabela de símbolos */
char varType(char name)
{
    char type;

    type = symbolType(name);
    if (!isVarType(type))
        notVar(name);

    return type;
}

/* analiza e traduz a declaração de uma variável */
void declaration()
{
    char type = look;
    nextChar();
    allocVar(getName(), type);
}

/* analiza e traduz as declarações globais */
void topDeclarations()
{
    while (look != 'B') {
        switch (look) {
            case 'b':
            case 'w':
            case 'l':
                declaration();
                break;
            default:
                unrecognized(look);
                break;
        }
        newLine();
    }
}

/* tratamento de operador unário */
char unaryOp()
{
    asmClear();
    return 'w';
}

/* analisa e traduz um fator matemático */
char factor()
{
    char type;

    if (look == '(') {
        match('(');
        type = expression();
        match(')');
    } else if (isalpha(look))
        type = loadVar(getName());
    else
        type = loadNum(getNum());

    return type;
}

/* reconhece e traduz uma multiplicação */
char multiply(char type)
{
    match('*');
    return asmPopMul(type, factor());
}

/* reconhece e traduz uma multiplicação */
char divide(char type)
{
    match('/');
    return asmPopDiv(type, factor());
}

/* analisa e traduz um termo matemático */
char term()
{
    char type;

    type = factor();
    while (isMulOp(look)) {
        asmPush(type);
        switch (look) {
            case '*':
                type = multiply(type);
                break;
            case '/':
                type = divide(type);
                break;
        }
    }

    return type;
}

/* reconhece e traduz uma soma */
char add(char type)
{
    match('+');
    return asmPopAdd(type, term());
}

/* reconhece e traduz uma subtração */
char subtract(char type)
{
    match('-');
    return asmPopSub(type, term());
}

/* analisa e traduz uma expressão */
char expression()
{
    char type;

    if (isAddOp(look))
        type = unaryOp();
    else
        type = term();
    while (isAddOp(look)) {
        asmPush(type);
        switch (look) {
            case '+':
                type = add(type);
                break;
            case '-':
                type = subtract(type);
                break;
        }
    }

    return type;
}

/* analisa e traduz uma atribuição */
void assignment()
{
    char name, type;

    name = getName();
    match('=');
    type = expression();
    storeVar(name, type);
}

/* analisa traduz um bloco de comandos */
void block()
{
    while (look != '.') {
        assignment();
        newLine();
    }
}