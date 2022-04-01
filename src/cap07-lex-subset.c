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

/* Tabela de definições de símbolos */
#define SYMBOLTABLE_SIZE 1000
char *SymbolTable[SYMBOLTABLE_SIZE];

/* Definição de palavras-chave e tipos de token */
#define KEYWORDLIST_SIZE 4
char *KeywordList[KEYWORDLIST_SIZE] = { "IF", "ELSE", "ENDIF", "END" };

/* A ordem deve obedecer a lista de palavras-chave */
const char *KeywordCode = "ilee";

char Token;
char TokenText[MAXTOKEN + 1];

char Look; /* O caractere lido "antecipadamente" (lookahead) */
int LabelCount; /* Contador usado pelo gerador de rótulos */

                /* Protótipos */
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

/* Programa principal */
int main()
{
    Init();
    Program();
    
    return 0;
}

/* Inicialização do compilador */
void Init()
{
    LabelCount = 0;
    NextChar();
    SkipWhite();
}

/* Lê próximo caractere da entrada */
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

/* Verifica se o caractere combina com o esperado */
void Match(char c)
{
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
    SkipWhite();
}

/* Verifica se a string combina com o esperado */
void MatchString(char *s)
{
    if (strcmp(TokenText, s) != 0)
        Expected("'%s'", s);
}

/* Recebe o nome de um identificador */
void GetName()
{
    int i;

    while (Look == '\n')
        NewLine();
    if (!isalpha(Look))
        Expected("Name");
    for (i = 0; isalnum(Look) && i < MAXNAME; i++)
    {
        TokenText[i] = toupper(Look);
        NextChar();
    }
    TokenText[i] = '\0';
    Token = 'x';
    SkipWhite();
}

/* Recebe um número inteiro */
void GetNum()
{
    int i;

    if (!isdigit(Look))
        Expected("Integer");
    for (i = 0; isdigit(Look) && i < MAXNUM; i++)
    {
        TokenText[i] = Look;
        NextChar();
    }
    TokenText[i] = '\0';
    Token = '#';
    SkipWhite();
}

/* Testa se caractere é um operador */
int IsOp(char c)
{
    return (strchr("+-*/<>:=", c) != NULL);
}

/* Recebe um operador */
void GetOp()
{
    int i;

    if (!IsOp(Look))
        Expected("Operator");
    for (i = 0; IsOp(Look) && i < MAXOP; i++)
    {
        TokenText[i] = Look;
        NextChar();
    }
    TokenText[i] = '\0';
    if (strlen(TokenText) == 1)
        Token = TokenText[0];
    else
        Token = '?';
}

/* Analisador léxico */
void Scan()
{
    int kw;

    GetName();
    kw = Lookup(TokenText, KeywordList, KEYWORDLIST_SIZE);
    if (kw != -1)
        Token = KeywordCode[kw];
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

/* Reconhece uma linha em branco */
void NewLine()
{
    if (Look == '\n')
        NextChar();
}

/* Pula caracteres de espaço */
void SkipWhite()
{
    while (Look == ' ' || Look == '\t')
        NextChar();
}

/* Reconhece operador aditivo */
int IsAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* Reconhece operador multiplicativo */
int IsMulOp(char c)
{
    return (c == '*' || c == '/');
}

/* Gera um novo rótulo único */
int NewLabel()
{
    return LabelCount++;
}

/* Emite um rótulo */
void PostLabel(int lbl)
{
    printf("L%d:\n", lbl);
}

/* Se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
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

/* Analisa e traduz um identificador */
void Ident()
{
    GetName();
    if (Look == '(')
    {
        Match('(');
        Match(')');
        EmitLn("CALL %s", TokenText);
    }
    else
        EmitLn("MOV AX, [%s]", TokenText);
}

/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name[MAXNAME + 1];
    strcpy(name, TokenText);
    Match('=');
    Expression();
    EmitLn("MOV [%s], AX", name);
}

/* Analisa e traduz um fator matemático */
void Factor()
{
    if (Look == '(')
    {
        Match('(');
        Expression();
        Match(')');
    }
    else if (isalpha(Look))
        Ident();
    else
    {
        GetNum();
        EmitLn("MOV AX, %s", TokenText);
    }
}

/* Analisa e traduz um fator com sinal opcional */
void SignedFactor()
{
    int minusSign = (Look == '-');
    if (IsAddOp(Look))
    {
        NextChar();
        SkipWhite();
    }
    Factor();
    if (minusSign)
        EmitLn("NEG AX");
}

/* Reconhece e traduz uma soma */
void Add()
{
    Match('+');
    Term();
    EmitLn("POP BX");
    EmitLn("ADD AX, BX");
}

/* Reconhece e traduz uma subtração */
void Subtract()
{
    Match('-');
    Term();
    EmitLn("POP BX");
    EmitLn("SUB AX, BX");
    EmitLn("NEG AX");
}

/* Reconhece e traduz uma multiplicação */
void Multiply()
{
    Match('*');
    Factor();
    EmitLn("POP BX");
    EmitLn("IMUL BX");
}

/* Reconhece e traduz uma divisão */
void Divide()
{
    Match('/');
    Factor();
    EmitLn("POP BX");
    EmitLn("XCHG AX, BX");
    EmitLn("CWD");
    EmitLn("IDIV BX");
}

/* Código comum usado por "term" e "firstTerm" */
void TermCommon()
{
    while (IsMulOp(Look))
    {
        EmitLn("PUSH AX");
        switch (Look)
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

/* Analisa e traduz um termo matemático no início de uma expressão */
void FirstTerm()
{
    SignedFactor();
    TermCommon();
}

/* Analisa e traduz um termo matemático */
void Term()
{
    Factor();
    TermCommon();
}

/* Analisa e traduz uma expressão matemática */
void Expression()
{
    FirstTerm();
    while (IsAddOp(Look))
    {
        EmitLn("PUSH AX");
        switch (Look)
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

/* Analisa e traduz uma condição */
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
    if (Token == 'l')
    {
        l2 = NewLabel();
        EmitLn("JMP L%d", l2);
        PostLabel(l1);
        Block();
    }
    PostLabel(l2);
    MatchString("ENDIF");
}

/* Analisa e traduz um bloco de comandos */
void Block()
{
    int follow = 0;

    Scan();
    while (Token != 'e' && Token != 'l')
    {
        switch (Token)
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

/* Analisa e traduz um programa completo */
void Program()
{
    Block();
    MatchString("END");
    EmitLn("; END");
}