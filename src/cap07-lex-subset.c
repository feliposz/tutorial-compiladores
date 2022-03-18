/*
Juntando os analisadores: Segundo passo - Analisador léxico

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

#define MAXNAME 30
#define MAXNUM 5
#define MAXOP 2
#define MAXTOKEN 30

/* tabela de definições de símbolos */
#define SYMTBL_SZ 1000
char *symtbl[SYMTBL_SZ];

/* definição de palavras-chave e tipos de token */
#define KWLIST_SZ 4
char *kwlist[KWLIST_SZ] = { "IF", "ELSE", "ENDIF", "END" };

/* a ordem deve obedecer a lista de palavras-chave */
const char *kwcode = "ilee";

char token;
char value[MAXTOKEN + 1];

char look; /* O caracter lido "antecipadamente" (lookahead) */
int labelCount; /* Contador usado pelo gerador de rótulos */

                /* protótipos */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
void matchString(char *s);
void getName();
void getNum();
void emit(char *fmt, ...);
void newLine();
void skipWhite();

int isAddOp(char c);
int isMulOp(char c);

int newLabel();
void postLabel(int lbl);

int lookup(char *s, char *list[], int size);
void scan();

void ident();
void factor();
void signedFactor();
void add();
void subtract();
void multiply();
void divide();
void termCommon();
void firstTerm();
void term();
void expression();
void assignment();
void condition();

void doIf();
void block();
void program();

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    program();
    
    return 0;
}

/* inicialização do compilador */
void init()
{
    labelCount = 0;
    nextChar();
    skipWhite();
}

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

/* verifica se o caracter combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();
    skipWhite();
}

/* verifica se a string combina com o esperado */
void matchString(char *s)
{
    if (strcmp(value, s) != 0)
        expected("'%s'", s);
}

/* recebe o nome de um identificador */
void getName()
{
    int i;

    while (look == '\n')
        newLine();
    if (!isalpha(look))
        expected("Name");
    for (i = 0; isalnum(look) && i < MAXNAME; i++)
    {
        value[i] = toupper(look);
        nextChar();
    }
    value[i] = '\0';
    token = 'x';
    skipWhite();
}

/* recebe um número inteiro */
void getNum()
{
    int i;

    if (!isdigit(look))
        expected("Integer");
    for (i = 0; isdigit(look) && i < MAXNUM; i++)
    {
        value[i] = look;
        nextChar();
    }
    value[i] = '\0';
    token = '#';
    skipWhite();
}

/* testa se caracter é um operador */
int isOp(char c)
{
    return (strchr("+-*/<>:=", c) != NULL);
}

/* recebe um operador */
void getOp()
{
    int i;

    if (!isOp(look))
        expected("Operator");
    for (i = 0; isOp(look) && i < MAXOP; i++)
    {
        value[i] = look;
        nextChar();
    }
    value[i] = '\0';
    if (strlen(value) == 1)
        token = value[0];
    else
        token = '?';
}

/* analisador léxico */
void scan()
{
    int kw;

    getName();
    kw = lookup(value, kwlist, KWLIST_SZ);
    if (kw != -1)
        token = kwcode[kw];
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

/* reconhece uma linha em branco */
void newLine()
{
    if (look == '\n')
        nextChar();
}

/* pula caracteres de espaço */
void skipWhite()
{
    while (look == ' ' || look == '\t')
        nextChar();
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

/* se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}

/* analisa e traduz um identificador */
void ident()
{
    getName();
    if (look == '(')
    {
        match('(');
        match(')');
        emit("CALL %s", value);
    }
    else
        emit("MOV AX, [%s]", value);
}

/* analisa e traduz um comando de atribuição */
void assignment()
{
    char name[MAXNAME + 1];
    strcpy(name, value);
    match('=');
    expression();
    emit("MOV [%s], AX", name);
}

/* analisa e traduz um fator matemático */
void factor()
{
    if (look == '(')
    {
        match('(');
        expression();
        match(')');
    }
    else if (isalpha(look))
        ident();
    else
    {
        getNum();
        emit("MOV AX, %s", value);
    }
}

/* analisa e traduz um fator com sinal opcional */
void signedFactor()
{
    int minusSign = (look == '-');
    if (isAddOp(look))
    {
        nextChar();
        skipWhite();
    }
    factor();
    if (minusSign)
        emit("NEG AX");
}

/* reconhece e traduz uma soma */
void add()
{
    match('+');
    term();
    emit("POP BX");
    emit("ADD AX, BX");
}

/* reconhece e traduz uma subtração */
void subtract()
{
    match('-');
    term();
    emit("POP BX");
    emit("SUB AX, BX");
    emit("NEG AX");
}

/* reconhece e traduz uma multiplicação */
void multiply()
{
    match('*');
    factor();
    emit("POP BX");
    emit("IMUL BX");
}

/* reconhece e traduz uma divisão */
void divide()
{
    match('/');
    factor();
    emit("POP BX");
    emit("XCHG AX, BX");
    emit("CWD");
    emit("IDIV BX");
}

/* código comum a firstTerm e term */
void termCommon()
{
    while (isMulOp(look))
    {
        emit("PUSH AX");
        switch (look)
        {
            case '*':
                multiply();
                break;
            case '/':
                divide();
                break;
        }
    }
}

/* analisa e traduz um termo matemático no início de uma expressão */
void firstTerm()
{
    signedFactor();
    termCommon();
}

/* analisa e traduz um termo matemático */
void term()
{
    factor();
    termCommon();
}

/* analisa e traduz uma expressão matemática */
void expression()
{
    firstTerm();
    while (isAddOp(look))
    {
        emit("PUSH AX");
        switch (look)
        {
            case '+':
                add();
                break;
            case '-':
                subtract();
                break;
        }
    }
}

/* analisa e traduz uma condição */
void condition()
{
    emit("; condition");
}

void doIf()
{
    int l1, l2;

    condition();
    l1 = newLabel();
    l2 = l1;
    emit("JZ L%d", l1);
    block();
    if (token == 'l')
    {
        l2 = newLabel();
        emit("JMP L%d", l2);
        postLabel(l1);
        block();
    }
    postLabel(l2);
    matchString("ENDIF");
}

/* analisa e traduz um bloco de comandos */
void block()
{
    int follow = 0;

    scan();
    while (token != 'e' && token != 'l')
    {
        switch (token)
        {
            case 'i':
                doIf();
                break;
            default:
                assignment();
                break;
        }
        scan();
    }
}

/* analisa e traduz um programa completo */
void program()
{
    block();
    matchString("END");
    emit("; END");
}