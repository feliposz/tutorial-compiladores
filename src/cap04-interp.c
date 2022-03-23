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

char Look; /* O caracter lido "antecipadamente" (lookahead) */

#define MAXVAR 26
int VarTable[MAXVAR];

/* Protótipos */
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

/* Programa principal */
int main()
{
    Init();
    do {
        switch (Look) {
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
    } while (Look != '.');

    return 0;
}

/* Inicialização do compilador */
void Init()
{
    InitVar();
    NextChar();
}

/* Inicializa variáveis */
void InitVar()
{
    int i;

    for (i = 0; i < MAXVAR; i++)
        VarTable[i] = 0;
}

/* Lê próximo caracter da entrada */
void NextChar()
{
    Look = getchar();
}

/* Captura um caracter de nova linha */
void NewLine()
{
    if (Look == '\n')
        NextChar();
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

/* Verifica se entrada combina com o esperado */
void Match(char c)
{
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
}

/* Recebe o nome de um identificador */
char GetName()
{
    char name;

    if (!isalpha(Look))
        Expected("Name");
    name = toupper(Look);
    NextChar();

    return name;
}

/* Recebe um número inteiro */
int GetNum()
{
    int i;

    i = 0;

    if (!isdigit(Look))
        Expected("Integer");

    while (isdigit(Look)) {
        i *= 10;
        i += Look - '0';
        NextChar();
    }

    return i;
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

/* Avalia um comando de atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    VarTable[name - 'A'] = Expression();
}

/* Avalia um fator */
int Factor()
{
    int val;

    if (Look == '(') {
        Match('(');
        val = Expression();
        Match(')');
    } else if (isalpha(Look))
        val = VarTable[GetName() - 'A'];
    else
        val = GetNum();

    return val;
}

/* Avalia um termo */
int Term()
{
    int val;

    val = Factor();
    while (IsMulOp(Look)) {
        switch (Look) {
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

/* Avalia o resultado de uma expressão */
int Expression()
{
    int val;

    if (IsAddOp(Look))
        val = 0;
    else
        val = Term();

    while (IsAddOp(Look)) {
        switch (Look) {
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

/* Interpreta um comando de entrada */
void Input()
{
    char name;
    char buffer[20];

    Match('?');
    name = GetName();
    printf("%c? ", name);
    fgets(buffer, 20, stdin);
    VarTable[name - 'A'] = atoi(buffer);
}

/* Interpreta um comando de saída */
void Output()
{
    char name;

    Match('!');
    name = GetName();
    printf("%c -> %d\n", name, VarTable[name - 'A']);
}
