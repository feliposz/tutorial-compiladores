/*
Tiny 1.3 (Alternativa) - Trata comentários de linha iniciados por #

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#define SYMTBL_SZ 1000
char *symbolTable[SYMTBL_SZ]; /* tabela de símbolos */
char symbolType[SYMTBL_SZ]; /* tabela de tipos de símbolos */
int symbolCount; /* número de entradas na tabela de símbolos */

char look; /* O caracter lido "antecipadamente" (lookahead) */
int labelCount; /* Contador usado pelo gerador de rótulos */

#define KWLIST_SZ 9

/* lista de palavras-chave */
char *kwlist[KWLIST_SZ] = {"IF", "ELSE", "ENDIF", "WHILE", "ENDWHILE",
                           "READ", "WRITE", "VAR", "END"};

/* códigos de palavras-chave: a ordem deve obedecer a lista de palavras-chave */
char *kwcode = "ileweRWve";

#define MAXTOKEN 16
char value[MAXTOKEN+1]; /* texto do token atual */
char token; /* código do token atual */

/* lê próximo caracter da entrada */
void nextChar()
{
    look = getchar();
}

/* pula um campo de comentário de uma só linha*/
void skipComment()
{
    do {
        nextChar();
    } while (look != '\n');
    nextChar();
}

/* pula caracteres em branco */
void skipWhite()
{
    while (isspace(look) || look == '#') {
        if (look == '#')
            skipComment();
        else
            nextChar();
    }
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

/* avisa a respeito de um identificador desconhecido */
void undefined(char *name)
{
    fatal("Error: Undefined identifier %s\n", name);
}

/* avisa a respeito de um identificador duplicado */
void duplicated(char *name)
{
    fatal("Error: Duplicated identifier %s\n", name);
}

/* reporta um erro se token NÃO for um identificador */
void checkIdent()
{
    if (token != 'x')
        expected("Identifier");
}

/* recebe o nome de um identificador ou palavra-chave */
void getName()
{
    int i;

    skipWhite();
    if (!isalpha(look))
        expected("Identifier or Keyword");
    for (i = 0; isalnum(look) && i < MAXTOKEN; i++) {
        value[i] = toupper(look);
        nextChar();
    }
    value[i] = '\0';
    token = 'x';
}

/* recebe um número inteiro */
void getNum()
{
    int i;

    skipWhite();
    if (!isdigit(look))
        expected("Integer");
    for (i = 0; isdigit(look) && i < MAXTOKEN; i++) {
        value[i] = look;
        nextChar();
    }
    value[i] = '\0';
    token = '#';
}

/* analisa e traduz um operador */
void getOp()
{
    skipWhite();
    token = look;
    value[0] = look;
    value[1] = '\0';
    nextChar();
}

/* pega o próximo token de entrada */
void nextToken()
{
    skipWhite();
    if (isalpha(look))
        getName();
    else if (isdigit(look))
        getNum();
    else
        getOp();
}

/* se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}

/* analisador léxico. analisa identificador ou palavra-chave */
void scan()
{
    int kw;

    if (token == 'x') {
        kw = lookup(value, kwlist, KWLIST_SZ);
        if (kw >= 0)
            token = kwcode[kw];
    }
}

/* verifica se a string combina com o esperado */
void matchString(char *s)
{
    if (strcmp(value, s) != 0)
        expected(s);
    nextToken();
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

/* reconhece operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* reconhece operador multiplicativo */
int isMulOp(char c)
{
    return (c == '*' || c == '/');
}

/* reconhece um operador OU */
int isOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* reconhece operadores relacionais */
int isRelOp(char c)
{
    return (c == '=' || c == '#' || c == '<' || c == '>');
}

/* verifica se símbolo está na tabela */
int inTable(char *name)
{
    return (lookup(name, symbolTable, symbolCount) >= 0);
}

/* retorna o endereço do identificador na tabela de símbolos */
int locate(char *name)
{
    return lookup(name, symbolTable, symbolCount);
}

/* reporta um erro se identificador NÃO constar na tabela de símbolos */
void checkTable(char *name)
{
    if (!inTable(name))
        undefined(name);
}

/* reporta um erro se identificador JÁ constar na tabela de símbolos */
void checkDuplicated(char *name)
{
    if (inTable(name))
        duplicated(name);
}

/* adiciona uma nova entrada à tabela de símbolos */
void addSymbol(char *name, char type)
{
    char *newSymbol;

    checkDuplicated(name);

    if (symbolCount >= SYMTBL_SZ) {
        fatal("Symbol table full!");
    }

    newSymbol = (char *) malloc(sizeof(char) * (strlen(name) + 1));
    if (newSymbol == NULL)
        fatal("Out of memory!");

    strcpy(newSymbol, name);

    symbolTable[symbolCount] = newSymbol;
    symbolType[symbolCount] = type;
    symbolCount++;
}

/* gera um novo rótulo único */
int newLabel()
{
    return labelCount++;
}

/* emite um rótulo */
void postLabel(int lbl)
{
    printf("L%d:\n", lbl);
}

/* cabeçalho inicial para o montador */
void header()
{
    emit(".model small");
    emit(".stack");
    emit(".code");
    printf("extrn READ:near, WRITE:near\n");
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

/* zera o registrador primário */
void asmClear()
{
    emit("XOR AX, AX");
}

/* negativa o registrador primário */
void asmNegative()
{
    emit("NEG AX");
}

/* carrega uma constante numérica no registrador primário */
void asmLoadConst(char *num)
{
    emit("MOV AX, %s", num);
}

/* carrega uma variável no registrador primário */
void asmLoadVar(char *name)
{
    emit("MOV AX, WORD PTR %s", name);
}

/* armazena registrador primário em variável */
void asmStore(char *name)
{
    emit("MOV WORD PTR %s, AX", name);
}

/* coloca registrador primário na pilha */
void asmPush()
{
    emit("PUSH AX");
}

/* adiciona o topo da pilha ao registrador primário */
void asmPopAdd()
{
    emit("POP BX");
    emit("ADD AX, BX");
}

/* subtrai o registrador primário do topo da pilha */
void asmPopSub()
{
    emit("POP BX");
    emit("SUB AX, BX");
    emit("NEG AX");
}

/* multiplica o topo da pilha pelo registrador primário */
void asmPopMul()
{
    emit("POP BX");
    emit("IMUL BX");
}

/* divide o topo da pilha pelo registrador primário */
void asmPopDiv()
{
    emit("POP BX");
    emit("XCHG AX, BX");
    emit("CWD");
    emit("IDIV BX");
}

/* inverte registrador primário */
void asmNot()
{
    emit("NOT AX");
}

/* aplica "E" binário ao topo da pilha com registrador primário */
void asmPopAnd()
{
    emit("POP BX");
    emit("AND AX, BX");
}

/* aplica "OU" binário ao topo da pilha com registrador primário */
void asmPopOr()
{
    emit("POP BX");
    emit("OR AX, BX");
}

/* aplica "OU-exclusivo" binário ao topo da pilha com registrador primário */
void asmPopXor()
{
    emit("POP BX");
    emit("XOR AX, BX");
}

/* compara topo da pilha com registrador primário */
void asmPopCompare()
{
    emit("POP BX");
    emit("CMP BX, AX");
}

/* altera registrador primário (e flags, indiretamente) conforme a comparação */
void asmRelOp(char op)
{
    char *jump;
    int l1, l2;

    l1 = newLabel();
    l2 = newLabel();

    switch (op) {
        case '=': jump = "JE"; break;
        case '#': jump = "JNE"; break;
        case '<': jump = "JL"; break;
        case '>': jump = "JG"; break;
        case 'L': jump = "JLE"; break;
        case 'G': jump = "JGE"; break;
    }

    emit("%s L%d", jump, l1);
    emit("XOR AX, AX");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* desvio incondicional */
void asmJmp(int label)
{
    emit("JMP L%d", label);
}

/* desvio se falso (0) */
void asmJmpFalse(int label)
{
    emit("JZ L%d", label);
}

/* lê um valor a partir da entrada e armazena-o no registrador primário */
void asmRead()
{
    emit("CALL READ");
}

/* mostra valor do registrador primário */
void asmWrite()
{
    emit("CALL WRITE");
}

/* alocação de memória para uma variável global */
void allocVar(char *name, int value)
{
    printf("%s:\tdw %d\n", name, value);
}

/* analisa uma lista de declaração de variáveis */
void declaration()
{
    nextToken();
    checkIdent();
    checkDuplicated(value);
    addSymbol(value, 'v');
    allocVar(value, 0);
    nextToken();
}

/* reconhece um ponto-e-vírgula opcional */
void semicolon()
{
    if (token == ';')
        matchString(";");
}

/* analisa e traduz declarações globais */
void topDeclarations()
{
    scan();
    while (token == 'v') {
        do {
            declaration();
        } while (token == ',');
        semicolon();
        scan();
    }
}

void boolExpression(); /* declaração adianta */

/* analisa e traduz um fator matemático */
void factor()
{
    if (token == '(') {
        nextToken();
        boolExpression();
        matchString(")");
    } else {
        if (token == 'x') {
            checkTable(value);
            asmLoadVar(value);
        } else if (token == '#')
            asmLoadConst(value);
        else
            expected("Math Factor");
        nextToken();
    }
}

/* reconhece e traduz uma multiplicação */
void multiply()
{
    nextToken();
    factor();
    asmPopMul();
}

/* reconhece e traduz uma divisão */
void divide()
{
    nextToken();
    factor();
    asmPopDiv();
}

/* analisa e traduz um termo matemático */
void term()
{
    factor();
    while (isMulOp(token)) {
        asmPush();
        switch (token) {
          case '*':
            multiply();
            break;
          case '/':
            divide();
            break;
        }
    }
}

/* reconhece e traduz uma adição */
void add()
{
    nextToken();
    term();
    asmPopAdd();
}

/* reconhece e traduz uma subtração*/
void subtract()
{
    nextToken();
    term();
    asmPopSub();
}

/* analisa e traduz uma expressão matemática */
void expression()
{
    if (isAddOp(token))
        asmClear();
    else
        term();
    while (isAddOp(token)) {
        asmPush();
        switch (token) {
            case '+':
                add();
                break;
            case '-':
                subtract();
                break;
        }
    }
}

/* analisa e traduz uma relação */
void relation()
{
    char op;

    expression();
    if (isRelOp(token)) {
        op = token;
        nextToken(); /* só para remover o operador do caminho */
        if (op == '<') {
            if (token == '>') { /* trata operador <> */
                nextToken();
                op = '#';
            } else if (token == '=') { /* trata operador <= */
                nextToken();
                op = 'L';
            }
        } else if (op == '>' && token == '=') { /* trata operador >= */
            nextToken();
            op = 'G';
        }
        asmPush();
        expression();
        asmPopCompare();
        asmRelOp(op);
    }
}

/* analisa e traduz um fator booleano com NOT inicial */
void notFactor()
{
    if (token == '!') {
        nextToken();
        relation();
        asmNot();
    } else
        relation();
}

/* analisa e traduz um termo booleano */
void boolTerm()
{
    notFactor();
    while (token == '&') {
        nextToken();
        asmPush();
        notFactor();
        asmPopAnd();
    }
}

/* reconhece e traduz um "OR" */
void boolOr()
{
    nextToken();
    boolTerm();
    asmPopOr();
}

/* reconhece e traduz um "xor" */
void boolXor()
{
    nextToken();
    boolTerm();
    asmPopXor();
}

/* analisa e traduz uma expressão booleana */
void boolExpression()
{
    boolTerm();
    while (isOrOp(token)) {
        asmPush();
        switch (token) {
          case '|':
              boolOr();
              break;
          case '~':
              boolXor();
              break;
        }
    }
}

/* analisa e traduz um comando de atribuição */
void assignment()
{
    char name[MAXTOKEN+1];

    strcpy(name, value);
    checkTable(name);
    nextToken();
    matchString("=");
    boolExpression();
    asmStore(name);
}

void block();

/* analisa e traduz um comando IF */
void doIf()
{
    int l1, l2;

    nextToken();
    boolExpression();
    l1 = newLabel();
    l2 = l1;
    asmJmpFalse(l1);
    block();
    if (token == 'l') {
        nextToken();
        l2 = newLabel();
        asmJmp(l2);
        postLabel(l1);
        block();
    }
    postLabel(l2);
    matchString("ENDIF");
}

/* analisa e traduz um comando WHILE */
void doWhile()
{
    int l1, l2;

    nextToken();
    l1 = newLabel();
    l2 = newLabel();
    postLabel(l1);
    boolExpression();
    asmJmpFalse(l2);
    block();
    matchString("ENDWHILE");
    asmJmp(l1);
    postLabel(l2);
}

/* processa um comando READ */
void doRead()
{
    nextToken();
    matchString("(");
    for (;;) {
        checkIdent();
        checkTable(value);
        asmRead();
        asmStore(value);
        nextToken();
        if (token != ',')
            break;
        nextToken();
    }
    matchString(")");
}

/* processa um comando WRITE */
void doWrite()
{
    nextToken();
    matchString("(");
    for (;;) {
        expression();
        asmWrite();
        if (token != ',')
            break;
        nextToken();
    }
    matchString(")");
}

/* Remova o comentário para usar o estilo "Pascal" de ponto-e-vírgulas */
/* #define PASCAL_STYLE */

#ifdef PASCAL_STYLE

/* analisa e traduz um único comando */
void statement()
{
    scan();
    switch (token) {
        case 'i':
            doIf();
            break;
        case 'w':
            doWhile();
            break;
        case 'R':
            doRead();
            break;
        case 'W':
            doWrite();
            break;
        case 'x':
            assignment();
            break;
    }
}

/* analiza e traduz um bloco de comandos estilo "Pascal" */
void block()
{
    statement();
    while (token == ';') {
        nextToken();
        statement();
    }
    scan();
}

#else

/* analisa e traduz um bloco de comandos estilo "C/Ada" */
void block()
{
    int follow = 0;

    do {
        scan();
        switch (token) {
            case 'i':
                doIf();
                break;
            case 'w':
                doWhile();
                break;
            case 'R':
                doRead();
                break;
            case 'W':
                doWrite();
                break;
            case 'x':
                assignment();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
        }
        if (!follow)
            semicolon();
    } while (!follow);
}

#endif

/* inicialização do compilador */
void init()
{
    symbolCount = 0;

    nextChar();
    nextToken();
}

/* PROGRAMA PRINCIPAL */
int main()
{
    init();

    matchString("PROGRAM");
    semicolon();
    header();
    topDeclarations();
    matchString("BEGIN");
    prolog();
    block();
    matchString("END");
    epilog();

    return 0;
}