/*
Análise Léxica (usando caracteres)

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
char *KeywordList[KEYWORDLIST_SIZE] = {"IF", "ELSE", "ENDIF", "END"};

/* a ordem deve obedecer a lista de palavras-chave */
const char *KeywordCode = "ilee";

char token;
char value[MAXTOKEN+1];
char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */
void Init();
void NextChar();
void Error(char *fmt, ...);
void Abort(char *fmt, ...);
void Expected(char *fmt, ...);
void SkipWhite();
void SkipComma();
void NewLine();
void Match(char c);
int IsOp(char c);
void EmitLn(char *fmt, ...);
void GetName();
void GetNum();
void GetOp();
void Scan();
int Lookup(char *s, char *list[], int size);

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();

    do {
        Scan();
        switch (token) {
            case 'x':
                printf("Ident: ");
                break;
            case '#':
                printf("Number: ");
                break;
            case 'i':
            case 'l':
            case 'e':
                printf("Keyword: ");
                break;
            default:
                printf("Operator: ");
                break;
        }
        printf("%s\n", value);
        if (value[0] == '\n')
            NewLine();
    } while (strcmp(value, "END") != 0);

    return 0;
}

/* inicialização do compilador */
void Init()
{
    NextChar();
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

/* pula caracteres de espaço */
void SkipWhite()
{
    while (look == ' ' || look == '\t')
        NextChar();
}

/* pular uma vírgula */
void SkipComma()
{
    SkipWhite();
    if (look == ',') {
        NextChar();
        SkipWhite();
    }
}

/* reconhece uma linha em branco */
void NewLine()
{
    if (look == '\n')
        NextChar();
}

/* verifica se entrada combina com o esperado */
void Match(char c)
{
    if (look != c)
        Expected("'%c'", c);
    NextChar();
}

/* testa se caracter é um operador */
int IsOp(char c)
{
    return (strchr("+-*/<>:=", c) != NULL);
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

/* recebe o nome de um identificador */
void GetName()
{
    int i, kw;

    if (!isalpha(look))
        Expected("Name");
    for (i = 0; isalnum(look) && i < MAXNAME; i++) {
        value[i] = toupper(look);
        NextChar();
    }
    value[i] = '\0';
    kw = Lookup(value, KeywordList, KEYWORDLIST_SIZE);
    if (kw == -1)
        token = 'x';
    else
        token = KeywordCode[kw];
}

/* recebe um número inteiro */
void GetNum()
{
    int i;

    if (!isdigit(look))
        Expected("Integer");
    for (i = 0; isdigit(look) && i < MAXNUM; i++) {
        value[i] = look;
        NextChar();
    }
    value[i] = '\0';
    token = '#';
}

/* recebe um operador */
void GetOp()
{
    int i;

    if (!IsOp(look))
        Expected("Operator");
    for (i = 0; IsOp(look) && i < MAXOP; i++) {
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
    while (look == '\n')
        NewLine();
    if (isalpha(look))
        GetName();
    else if (isdigit(look))
        GetNum();
    else if (IsOp(look))
        GetOp();
    else {
        value[0] = look;
        value[1] = '\0';
        token = '?';
        NextChar();
    }
    SkipWhite();
}

/* se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int Lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}