/*
Interpretadores

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

#define MAXVAR 26
int var[MAXVAR];

/* protótipos */
void Init();
void InitVar();
void NextChar();
void NewLine();
void Error(char *fmt, ...);
void Abort(char *fmt, ...);
void Expected(char *fmt, ...);
void Match(char c);
char GetName();
int GetNum();
void EmitLn(char *fmt, ...);
void Assignment();
int Factor();
int Term();
int Expression();
void Input();
void Output();
int IsAddOp(char c);
int IsMulOp(char c);

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    do {
        switch (look) {
            case '?':
                Input();
                break;
            case '!':
                Output();
                break;
            default:
                Assignment();
                break;
        }
        NewLine();
    } while (look != '.');

    return 0;
}

/* inicialização do compilador */
void Init()
{
    InitVar();
    NextChar();
}

/* inicializa variáveis */
void InitVar()
{
    int i;

    for (i = 0; i < MAXVAR; i++)
        var[i] = 0;
}

/* lê próximo caracter da entrada */
void NextChar()
{
    look = getchar();
}

/* captura um caracter de nova linha */
void NewLine()
{
    if (look == '\n')
        NextChar();
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

/* verifica se entrada combina com o esperado */
void Match(char c)
{
    if (look != c)
        Expected("'%c'", c);
    NextChar();
}

/* recebe o nome de um identificador */
char GetName()
{
    char name;

    if (!isalpha(look))
        Expected("Name");
    name = toupper(look);
    NextChar();

    return name;
}

/* recebe um número inteiro */
int GetNum()
{
    int i;

    i = 0;

    if (!isdigit(look))
        Expected("Integer");

    while (isdigit(look)) {
        i *= 10;
        i += look - '0';
        NextChar();
    }

    return i;
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

/* avalia um comando de atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    var[name - 'A'] = Expression();
}

/* avalia um fator */
int Factor()
{
    int val;

    if (look == '(') {
        Match('(');
        val = Expression();
        Match(')');
    } else if (isalpha(look))
        val = var[GetName() - 'A'];
    else
        val = GetNum();

    return val;
}

/* avalia um termo */
int Term()
{
    int val;

    val = Factor();
    while (IsMulOp(look)) {
        switch (look) {
            case '*':
                Match('*');
                val *= Factor();
                break;
            case '/':
                Match('/');
                val /= Factor();
                break;
        }
    }

    return val;
}

/* avalia o resultado de uma expressão */
int Expression()
{
    int val;

    if (IsAddOp(look))
        val = 0;
    else
        val = Term();

    while (IsAddOp(look)) {
        switch (look) {
          case '+':
              Match('+');
              val += Term();
              break;
          case '-':
              Match('-');
              val -= Term();
              break;
        }
    }

    return val;
}

/* interpreta um comando de entrada */
void Input()
{
    char name;
    char buffer[20];

    Match('?');
    name = GetName();
    printf("%c? ", name);
    fgets(buffer, 20, stdin);
    var[name - 'A'] = atoi(buffer);
}

/* interpreta um comando de saída */
void Output()
{
    char name;

    Match('!');
    name = GetName();
    printf("%c -> %d\n", name, var[name - 'A']);
}
