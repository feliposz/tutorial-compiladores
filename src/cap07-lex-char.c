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

/* Tabela de definições de símbolos */
#define SYMBOLTABLE_SIZE 1000
char *SymbolTable[SYMBOLTABLE_SIZE];

/* Definição de palavras-chave e tipos de token */
#define KEYWORDLIST_SIZE 4
char *KeywordList[KEYWORDLIST_SIZE] = {"IF", "ELSE", "ENDIF", "END"};

/* A ordem deve obedecer a lista de palavras-chave */
const char *KeywordCode = "ilee";

char Token;
char TokenText[MAXTOKEN+1];
char Look; /* O caracter lido "antecipadamente" (lookahead) */

/* Protótipos */
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

/* Programa principal */
int main()
{
    Init();

    do {
        Scan();
        switch (Token) {
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
        printf("%s\n", TokenText);
        if (TokenText[0] == '\n')
            NewLine();
    } while (strcmp(TokenText, "END") != 0);

    return 0;
}

/* Inicialização do compilador */
void Init()
{
    NextChar();
}

/* Lê próximo caracter da entrada */
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

/* Pula caracteres de espaço */
void SkipWhite()
{
    while (Look == ' ' || Look == '\t')
        NextChar();
}

/* Pular uma vírgula */
void SkipComma()
{
    SkipWhite();
    if (Look == ',') {
        NextChar();
        SkipWhite();
    }
}

/* Reconhece uma linha em branco */
void NewLine()
{
    if (Look == '\n')
        NextChar();
}

/* Verifica se entrada combina com o esperado */
void Match(char c)
{
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
}

/* Testa se caracter é um operador */
int IsOp(char c)
{
    return (strchr("+-*/<>:=", c) != NULL);
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

/* Recebe o nome de um identificador */
void GetName()
{
    int i, kw;

    if (!isalpha(Look))
        Expected("Name");
    for (i = 0; isalnum(Look) && i < MAXNAME; i++) {
        TokenText[i] = toupper(Look);
        NextChar();
    }
    TokenText[i] = '\0';
    kw = Lookup(TokenText, KeywordList, KEYWORDLIST_SIZE);
    if (kw == -1)
        Token = 'x';
    else
        Token = KeywordCode[kw];
}

/* Recebe um número inteiro */
void GetNum()
{
    int i;

    if (!isdigit(Look))
        Expected("Integer");
    for (i = 0; isdigit(Look) && i < MAXNUM; i++) {
        TokenText[i] = Look;
        NextChar();
    }
    TokenText[i] = '\0';
    Token = '#';
}

/* Recebe um operador */
void GetOp()
{
    int i;

    if (!IsOp(Look))
        Expected("Operator");
    for (i = 0; IsOp(Look) && i < MAXOP; i++) {
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
    while (Look == '\n')
        NewLine();
    if (isalpha(Look))
        GetName();
    else if (isdigit(Look))
        GetNum();
    else if (IsOp(Look))
        GetOp();
    else {
        TokenText[0] = Look;
        TokenText[1] = '\0';
        Token = '?';
        NextChar();
    }
    SkipWhite();
}

/* Se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int Lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}