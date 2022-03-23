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

int look; /* O caracter lido "antecipadamente" (lookahead) */
char CurrentClass; /* classe atual lida por getClass */
char CurrentType; /* tipo atual lido por getType */
char CurrentSigned; /* indica se tipo atual é com ou sem sinal */

/* lê próximo caracter da entrada */
void NextChar()
{
    look = getchar();
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
char GetNum()
{
    char num;

    if (!isdigit(look))
        Expected("Integer");
    num = look;
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
    if (look == 'a' || look == 'x' || look == 's') {
        CurrentClass = look;
        NextChar();
    } else
        CurrentClass = 'a';
}

/* recebe um tipo de dado C */
void GetType()
{
    CurrentType = ' ';
    if (look == 'u') {
        CurrentSigned = 'u';
        CurrentType = 'i';
        NextChar();
    } else {
        CurrentSigned = 's';
    }
    if (look == 'i' || look == 'l' || look == 'c') {
        CurrentType = look;
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
        if (look != ',')
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
    if (look == '(')
        FunctionDeclaration(name);
    else
        DoData(name);
}

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();

    while (look != EOF && look != '\n') {
        GetClass();
        GetType();
        TopDeclaration();
    }

    return 0;
}