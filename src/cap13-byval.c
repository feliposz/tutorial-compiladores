/*
Procedimentos - Passando parâmetros por valor

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

#define PARAMTBL_SZ 26
int paramTable[PARAMTBL_SZ]; /* lista de parâmetros formais para os procedimentos */
int paramCount; /* número de parâmetros formais */

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

/* tratamento da tabela de parâmetros formais */
void clearParams();
int paramPos(char name);
int isParam(char name);
void addParam(char name);

/* analisador léxico rudimentar */
int isAddOp(char c);
int isMulOp(char c);
void skipWhite();
void newLine();
void match(char c);
char getName();
char getNum();

/* geração de código */
void header();
void prolog();
void epilog();
void asmLoadVar(char name);
void asmStoreVar(char name);
void asmAllocVar(char name);
void asmCall(char name);
void asmReturn();
int asmOffsetParam(int par);
void asmLoadParam(int par);
void asmStoreParam(int par);
void asmPush();
void asmCleanstack(int bytes);
void asmProcProlog(char name);
void asmProcEpilog();

/* analisador sintático */
void expression();
void assignment(char name);
void assignOrCall();
void doBlock();
void beginBlock();
void param();
int paramList();
void doCallProc(char name);
void formalList();
void formalParam();
void doProcedure();
void declaration();
void topDeclarations();
void doMain();

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    header();
    topDeclarations();
    epilog();

    return 0;
}

/* inicialização do compilador */
void init()
{
    int i;

    for (i = 0; i < SYMTBL_SZ; i++)
        symbolTable[i] = ' ';

    clearParams();

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
    if (isParam(name))
        return 'f';
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

/* limpa a tabela de parâmetros formais */
void clearParams()
{
    int i;
    for (i = 0; i < PARAMTBL_SZ; i++)
        paramTable[i] = 0;
    paramCount = 0;
}

/* retorna número indicando a posição do parâmetro */
int paramPos(char name)
{
    return paramTable[name - 'A'];
}

/* verifica se nome é parâmetro */
int isParam(char name)
{
    return (paramTable[name - 'A'] != 0);
}

/* adiciona parâmetro à lista */
void addParam(char name)
{
    if (isParam(name))
        duplicated(name);
    paramTable[name - 'A'] = ++paramCount;
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

/* cabeçalho inicial para o montador */
void header()
{
    emit(".model small");
    emit(".stack");
    emit(".code");
    printf("PROG segment byte public\n");
    emit("assume cs:PROG,ds:PROG,es:PROG,ss:PROG");
}

/* emite código para o prólogo de um programa */
void prolog()
{
    printf("MAIN:\n");
    emit("MOV AX, PROG");
    emit("MOV DS, AX");
    emit("MOV ES, AX");
}

/* emite código para o epílogo de um programa */
void epilog()
{
    emit("MOV AX, 4C00h");
    emit("INT 21h");
    printf("PROG ends\n");
    emit("end MAIN");
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

/* gera uma chamada de procedimento */
void asmCall(char name)
{
    emit("CALL %c", name);
}

/* retorno de sub-rotina */
void asmReturn()
{
    emit("RET");
}

/* calcula deslocamento do parâmetro na pilha */
int asmOffsetParam(int par)
{
    int offset;

    /* offset = (endereço de retorno + BP) + tamanho do parâmetro * posição relativa */
    offset = 4 + 2 * (paramCount - par); 

    return offset;
}

/* carrega parâmetro em registrador primário */
void asmLoadParam(int par)
{
    int offset = asmOffsetParam(par);
    emit("MOV AX, WORD PTR [BP+%d]", offset);
}

/* armazena conteúdo do registrador primário em parâmetro */
void asmStoreParam(int par)
{
    int offset = asmOffsetParam(par);
    emit("MOV WORD PTR [BP+%d], AX", offset);
}

/* coloca registrador primário na pilha */
void asmPush()
{
    emit("PUSH AX");
}

/* ajusta o ponteiro da pilha acima */
void asmCleanstack(int bytes)
{
    if (bytes > 0)
        emit("ADD SP, %d", bytes);
}

/* escreve o prólogo para um procedimento */
void asmProcProlog(char name)
{
    printf("%c:\n", name);
    emit("PUSH BP");
    emit("MOV BP, SP");
}

/* escreve o epílogo para um procedimento */
void asmProcEpilog()
{
    emit("POP BP");
    emit("RET");
}

/* analisa e traduz uma expressão */
void expression()
{
    char name = getName();
    if (isParam(name))
        asmLoadParam(paramPos(name));
    else
        asmLoadVar(name);
}

/* analisa e traduz um comando de atribuição */
void assignment(char name)
{
    match('=');
    expression();
    if (isParam(name))
        asmStoreParam(paramPos(name));
    else
        asmStoreVar(name);
}

/* analisa e traduz um comando de atribuição ou chamada de procedimento */
void assignOrCall()
{
    char name;

    name = getName();
    switch (symbolType(name)) {
        case ' ':
            undefined(name);
            break;
        case 'v':
        case 'f':
            assignment(name);
            break;
        case 'p':
            doCallProc(name);
            break;
        default:
            fatal("Identifier %c cannot be used here!", name);
    }
}

/* analiza e traduz um bloco de comandos */
void doBlock()
{
    while (look != 'e') {
        assignOrCall();
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

/* processa um parâmetro de chamada */
void param()
{
    expression();
    asmPush();
}

/* processa a lista de parâmetros para uma chamada de procedimento */
int paramList()
{
    int count = 0;;

    match('(');
    if (look != ')') {
        for (;;) {
            param();
            count++;
            if (look != ',')
                break;
            match(',');
        }
    }
    match(')');

    return count * 2; /* número de parâmetros * bytes por parâmetro */
}

/* processa uma chamada de procedimento */
void doCallProc(char name)
{
    int bytes = paramList();
    asmCall(name);
    asmCleanstack(bytes);
}

/* processa um parâmetro formal */
void formalParam()
{
    char name;

    name = getName();
    addParam(name);
}

/* processa a lista de parâmetros formais de um procedimento */
void formalList()
{
    match('(');
    if (look != ')') {
        formalParam();
        while (look == ',') {
            match(',');
            formalParam();
        }
    }
    match(')');
    newLine();
}

/* analisa e traduz uma declaração de procedimento */
void doProcedure()
{
    char name;

    match('p');
    name = getName();
    addSymbol(name, 'p');
    formalList();
    asmProcProlog(name);
    beginBlock();
    asmProcEpilog();
    clearParams();
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
    while (look != '.') {
        switch (look) {
            case 'v':
                declaration();
                break;
            case 'p':
                doProcedure();
                break;
            case 'P':
                doMain();
                break;
            default:
                unrecognized(look);
                break;
        }
        newLine();
    }
}

/* analiza e traduz o bloco principal do programa */
void doMain()
{
    char name;

    match('P');
    name = getName();
    newLine();
    if (inTable(name))
        duplicated(name);
    prolog();
    beginBlock();
}