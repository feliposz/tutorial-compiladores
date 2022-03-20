/*
Tiny 0.1 - Tokens de um caracter

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#define VARTBL_SZ 26
char vartbl[VARTBL_SZ]; /* tabela de símbolos */

char look; /* O caracter lido "antecipadamente" (lookahead) */
int labelCount; /* Contador usado pelo gerador de rótulos */

/* lê próximo caracter da entrada */
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

/* verifica se entrada combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();
}

/* recebe o nome de um identificador */
char getName()
{
    char name;

    if (!isalpha(look))
        expected("Name");
    name = toupper(look);
    nextChar();

    return name;
}

/* recebe um número inteiro */
int getNum()
{
    int num;

    num = 0;

    if (!isdigit(look))
        expected("Integer");

    while (isdigit(look)) {
        num *= 10;
        num += look - '0';
        nextChar();
    }

    return num;
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

/* avisa a respeito de um identificador desconhecido */
void undefined(char name)
{
    fatal("Error: Undefined identifier %c\n", name);
}

/* verifica se símbolo está na tabela */
int inTable(char name)
{
    return (vartbl[name - 'A'] != ' ');
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
void asmLoadConst(int i)
{
    emit("MOV AX, %d", i);
}

/* carrega uma variável no registrador primário */
void asmLoadVar(char name)
{
    if (!inTable(name))
        undefined(name);
    emit("MOV AX, WORD PTR %c", name);
}

/* armazena registrador primário em variável */
void asmStore(char name)
{
    if (!inTable(name))
        undefined(name);
    emit("MOV WORD PTR %c, AX", name);
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

/* alocação de memória para uma variável global */
void allocVar(char name)
{
    int value = 0, signal = 1;

    if (inTable(name))
        fatal("Duplicate variable name: %c", name);
    else
        vartbl[name - 'A'] = 'v';

    if (look == '=') {
        match('=');
        if (look == '-') {
            match('-');
            signal = -1;
        }
        value = signal * getNum();
    }    

    printf("%c:\tdw %d\n", name, value);
}

/* analisa uma lista de declaração de variáveis */
void declaration()
{
    match('v');
    for (;;) {
        allocVar(getName());
        if (look != ',')
            break;
        match(',');
    }
}

/* analisa e traduz declarações globais */
void topDeclarations()
{
    while (look != 'b') {
        switch (look) {
            case 'v':
                declaration();
                break;
            default:
                error("Unrecognized keyword.");
                expected("BEGIN");
                break;
        }
    }
}

void boolExpression(); /* declaração adianta */

/* analisa e traduz um fator matemático */
void factor()
{
    if (look == '(') {
        match('(');
        boolExpression();
        match(')');
    } else if (isalpha(look))
        asmLoadVar(getName());
    else
        asmLoadConst(getNum());
}

/* analisa e traduz um fator negativo */
void negFactor()
{
    match('-');
    if (isdigit(look))
        asmLoadConst(-getNum());
    else {
        factor();
        asmNegative();
    }
}

/* analisa e traduz um fator inicial */
void firstFactor()
{
    switch (look) {
        case '+':
            match('+');
            factor();
            break;
        case '-':
            negFactor();
            break;
        default:
            factor();
            break;
    }
}

/* reconhece e traduz uma multiplicação */
void multiply()
{
    match('*');
    factor();
    asmPopMul();
}

/* reconhece e traduz uma divisão */
void divide()
{
    match('/');
    factor();
    asmPopDiv();
}

/* código comum usado por "term" e "firstTerm" */
void termCommon()
{
    while (isMulOp(look)) {
        asmPush();
        switch (look) {
          case '*':
            multiply();
            break;
          case '/':
            divide();
            break;
        }
    }
}

/* analisa e traduz um termo matemático */
void term()
{
    factor();
    termCommon();
}

/* analisa e traduz um termo inicial */
void firstTerm()
{
    firstFactor();
    termCommon();
}

/* reconhece e traduz uma adição */
void add()
{
    match('+');
    term();
    asmPopAdd();
}

/* reconhece e traduz uma subtração*/
void subtract()
{
    match('-');
    term();
    asmPopSub();
}

/* analisa e traduz uma expressão matemática */
void expression()
{
    firstTerm();
    while (isAddOp(look)) {
        asmPush();
        switch (look) {
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
    if (isRelOp(look)) {
        op = look;
        match(op); /* só para remover o operador do caminho */
        asmPush();
        expression();
        asmPopCompare();
        asmRelOp(op);
    }
}

/* analisa e traduz um fator booleano com NOT inicial */
void notFactor()
{
    if (look == '!') {
        match('!');
        relation();
        asmNot();
    } else
        relation();
}

/* analisa e traduz um termo booleano */
void boolTerm()
{
    notFactor();
    while (look == '&') {
        asmPush();
        match('&');
        notFactor();
        asmPopAnd();
    }
}

/* reconhece e traduz um "OR" */
void boolOr()
{
    match('|');
    boolTerm();
    asmPopOr();
}

/* reconhece e traduz um "xor" */
void boolXor()
{
    match('~');
    boolTerm();
    asmPopXor();
}

/* analisa e traduz uma expressão booleana */
void boolExpression()
{
    boolTerm();
    while (isOrOp(look)) {
        asmPush();
        switch (look) {
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
    char name;

    name = getName();
    match('=');
    boolExpression();
    asmStore(name);
}

void block();

/* analisa e traduz um comando IF */
void doIf()
{
    int l1, l2;

    match('i');
    boolExpression();
    l1 = newLabel();
    l2 = l1;
    asmJmpFalse(l1);
    block();
    if (look == 'l') {
        match('l');
        l2 = newLabel();
        asmJmp(l2);
        postLabel(l1);
        block();
    }
    postLabel(l2);
    match('e');
}

/* analisa e traduz um comando WHILE */
void doWhile()
{
    int l1, l2;

    match('w');
    l1 = newLabel();
    l2 = newLabel();
    postLabel(l1);
    boolExpression();
    asmJmpFalse(l2);
    block();
    match('e');
    asmJmp(l1);
    postLabel(l2);
}

/* analisa e traduz um bloco de comandos */
void block()
{
    int follow = 0;

    while (!follow) {
        switch (look) {
            case 'i':
                doIf();
                break;
            case 'w':
                doWhile();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                assignment();
                break;
        }
    }
}

/* analisa e traduz o bloco principal */
void mainBlock()
{
    match('b');
    prolog();
    block();
    match('e');
    epilog();
}

/* analisa e traduz um programa completo */
void program()
{
    match('p');
    header();
    topDeclarations();
    mainBlock();
    match('.');
}

/* inicialização do compilador */
void init()
{
    int i = 0;

    for (i = 0; i < VARTBL_SZ; i++)
        vartbl[i] = ' ';

    nextChar();
}

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    program();

    if (look != '\n')
        fatal("Unexpected data after \'.\'");

    return 0;
}