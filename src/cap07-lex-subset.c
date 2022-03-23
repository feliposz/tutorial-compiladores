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
#define SYMBOLTABLE_SIZE 1000
char *SymbolTable[SYMBOLTABLE_SIZE];

/* definição de palavras-chave e tipos de token */
#define KEYWORDLIST_SIZE 4
char *KeywordList[KEYWORDLIST_SIZE] = { "IF", "ELSE", "ENDIF", "END" };

/* a ordem deve obedecer a lista de palavras-chave */
const char *KeywordCode = "ilee";

char token;
char value[MAXTOKEN + 1];

char look; /* O caracter lido "antecipadamente" (lookahead) */
int LabelCount; /* Contador usado pelo gerador de rótulos */

                /* protótipos */
void Init();
void NextChar();
void Error(char *fmt, ...);
void Abort(char *fmt, ...);
void Expected(char *fmt, ...);
void Match(char c);
void MatchString(char *s);
void GetName();
void GetNum();
void EmitLn(char *fmt, ...);
void NewLine();
void SkipWhite();

int IsAddOp(char c);
int IsMulOp(char c);

int NewLabel();
void PostLabel(int lbl);

int Lookup(char *s, char *list[], int size);
void Scan();

void Ident();
void Factor();
void SignedFactor();
void Add();
void Subtract();
void Multiply();
void Divide();
void TermCommon();
void FirstTerm();
void Term();
void Expression();
void Assignment();
void Condition();

void DoIf();
void Block();
void Program();

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    Program();
    
    return 0;
}

/* inicialização do compilador */
void Init()
{
    LabelCount = 0;
    NextChar();
    SkipWhite();
}

/* lê próximo caracter da entrada */
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

/* verifica se o caracter combina com o esperado */
void Match(char c)
{
    if (look != c)
        Expected("'%c'", c);
    NextChar();
    SkipWhite();
}

/* verifica se a string combina com o esperado */
void MatchString(char *s)
{
    if (strcmp(value, s) != 0)
        Expected("'%s'", s);
}

/* recebe o nome de um identificador */
void GetName()
{
    int i;

    while (look == '\n')
        NewLine();
    if (!isalpha(look))
        Expected("Name");
    for (i = 0; isalnum(look) && i < MAXNAME; i++)
    {
        value[i] = toupper(look);
        NextChar();
    }
    value[i] = '\0';
    token = 'x';
    SkipWhite();
}

/* recebe um número inteiro */
void GetNum()
{
    int i;

    if (!isdigit(look))
        Expected("Integer");
    for (i = 0; isdigit(look) && i < MAXNUM; i++)
    {
        value[i] = look;
        NextChar();
    }
    value[i] = '\0';
    token = '#';
    SkipWhite();
}

/* testa se caracter é um operador */
int IsOp(char c)
{
    return (strchr("+-*/<>:=", c) != NULL);
}

/* recebe um operador */
void GetOp()
{
    int i;

    if (!IsOp(look))
        Expected("Operator");
    for (i = 0; IsOp(look) && i < MAXOP; i++)
    {
        value[i] = look;
        NextChar();
    }
    value[i] = '\0';
    if (strlen(value) == 1)
        token = value[0];
    else
        token = '?';
}

/* analisador léxico */
void Scan()
{
    int kw;

    GetName();
    kw = Lookup(value, KeywordList, KEYWORDLIST_SIZE);
    if (kw != -1)
        token = KeywordCode[kw];
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

/* reconhece uma linha em branco */
void NewLine()
{
    if (look == '\n')
        NextChar();
}

/* pula caracteres de espaço */
void SkipWhite()
{
    while (look == ' ' || look == '\t')
        NextChar();
}

/* reconhece operador aditivo */
int IsAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* reconhece operador multiplicativo */
int IsMulOp(char c)
{
    return (c == '*' || c == '/');
}

/* gera um novo rótulo único */
int NewLabel()
{
    return LabelCount++;
}

/* emite um rótulo */
void PostLabel(int lbl)
{
    printf("L%d:\n", lbl);
}

/* se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int Lookup(char *s, char *list[], int size)
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
void Ident()
{
    GetName();
    if (look == '(')
    {
        Match('(');
        Match(')');
        EmitLn("CALL %s", value);
    }
    else
        EmitLn("MOV AX, [%s]", value);
}

/* analisa e traduz um comando de atribuição */
void Assignment()
{
    char name[MAXNAME + 1];
    strcpy(name, value);
    Match('=');
    Expression();
    EmitLn("MOV [%s], AX", name);
}

/* analisa e traduz um fator matemático */
void Factor()
{
    if (look == '(')
    {
        Match('(');
        Expression();
        Match(')');
    }
    else if (isalpha(look))
        Ident();
    else
    {
        GetNum();
        EmitLn("MOV AX, %s", value);
    }
}

/* analisa e traduz um fator com sinal opcional */
void SignedFactor()
{
    int minusSign = (look == '-');
    if (IsAddOp(look))
    {
        NextChar();
        SkipWhite();
    }
    Factor();
    if (minusSign)
        EmitLn("NEG AX");
}

/* reconhece e traduz uma soma */
void Add()
{
    Match('+');
    Term();
    EmitLn("POP BX");
    EmitLn("ADD AX, BX");
}

/* reconhece e traduz uma subtração */
void Subtract()
{
    Match('-');
    Term();
    EmitLn("POP BX");
    EmitLn("SUB AX, BX");
    EmitLn("NEG AX");
}

/* reconhece e traduz uma multiplicação */
void Multiply()
{
    Match('*');
    Factor();
    EmitLn("POP BX");
    EmitLn("IMUL BX");
}

/* reconhece e traduz uma divisão */
void Divide()
{
    Match('/');
    Factor();
    EmitLn("POP BX");
    EmitLn("XCHG AX, BX");
    EmitLn("CWD");
    EmitLn("IDIV BX");
}

/* código comum a firstTerm e term */
void TermCommon()
{
    while (IsMulOp(look))
    {
        EmitLn("PUSH AX");
        switch (look)
        {
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
        }
    }
}

/* analisa e traduz um termo matemático no início de uma expressão */
void FirstTerm()
{
    SignedFactor();
    TermCommon();
}

/* analisa e traduz um termo matemático */
void Term()
{
    Factor();
    TermCommon();
}

/* analisa e traduz uma expressão matemática */
void Expression()
{
    FirstTerm();
    while (IsAddOp(look))
    {
        EmitLn("PUSH AX");
        switch (look)
        {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
        }
    }
}

/* analisa e traduz uma condição */
void Condition()
{
    EmitLn("; condition");
}

void DoIf()
{
    int l1, l2;

    Condition();
    l1 = NewLabel();
    l2 = l1;
    EmitLn("JZ L%d", l1);
    Block();
    if (token == 'l')
    {
        l2 = NewLabel();
        EmitLn("JMP L%d", l2);
        PostLabel(l1);
        Block();
    }
    PostLabel(l2);
    MatchString("ENDIF");
}

/* analisa e traduz um bloco de comandos */
void Block()
{
    int follow = 0;

    Scan();
    while (token != 'e' && token != 'l')
    {
        switch (token)
        {
            case 'i':
                DoIf();
                break;
            default:
                Assignment();
                break;
        }
        Scan();
    }
}

/* analisa e traduz um programa completo */
void Program()
{
    Block();
    MatchString("END");
    EmitLn("; END");
}