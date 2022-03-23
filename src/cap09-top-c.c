/*
Uma visão do alto - Compilador "C"

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

int Look; /* O caracter lido "antecipadamente" (lookahead) */
char CurrentClass; /* classe atual lida por getClass */
char CurrentType; /* tipo atual lido por getType */
char CurrentSigned; /* indica se tipo atual é com ou sem sinal */

/* lê próximo caracter da entrada */
void NextChar()
{
    Look = getchar();
}

/* inicialização do compilador */
void Init()
{
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
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
}

/* recebe o nome de um identificador */
char GetName()
{
    char name;

    if (!isalpha(Look))
        Expected("Name");
    name = toupper(Look);
    NextChar();

    return name;
}

/* recebe um número inteiro */
char GetNum()
{
    char num;

    if (!isdigit(Look))
        Expected("Integer");
    num = Look;
    NextChar();

    return num;
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

/* recebe uma classe de armazenamento C */
void GetClass()
{
    if (Look == 'a' || Look == 'x' || Look == 's') {
        CurrentClass = Look;
        NextChar();
    } else
        CurrentClass = 'a';
}

/* recebe um tipo de dado C */
void GetType()
{
    CurrentType = ' ';
    if (Look == 'u') {
        CurrentSigned = 'u';
        CurrentType = 'i';
        NextChar();
    } else {
        CurrentSigned = 's';
    }
    if (Look == 'i' || Look == 'l' || Look == 'c') {
        CurrentType = Look;
        NextChar();
    }
}

/* analisa uma declaração de função */
void FunctionDeclaration(char name)
{
    Match('(');
    Match(')');
    Match('{');
    Match('}');
    if (CurrentType == ' ')
        CurrentType = 'i';
    printf("Class: %c, Sign: %c, Type: %c, Function: %c\n",
        CurrentClass, CurrentSigned, CurrentType, name);
}

/* analisa uma declaração de variável */
void DoData(char name)
{
    if (CurrentType == ' ')
        Expected("Type declaration");
    for (;;) {
        printf("Class: %c, Sign: %c, Type: %c, Data: %c\n",
            CurrentClass, CurrentSigned, CurrentType, name);
        if (Look != ',')
            break;
        Match(',');
        name = GetName();
    }
    Match(';');
}

/* analisa uma declaração */
void TopDeclaration()
{
    char name;

    name = GetName();
    if (Look == '(')
        FunctionDeclaration(name);
    else
        DoData(name);
}

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();

    while (Look != EOF && Look != '\n') {
        GetClass();
        GetType();
        TopDeclaration();
    }

    return 0;
}