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
char CurrentClass; /* Classe atual lida por getClass */
char CurrentType; /* Tipo atual lido por getType */
char CurrentSigned; /* Indica se tipo atual é com ou sem sinal */

/* Lê próximo caracter da entrada */
void NextChar()
{
    Look = getchar();
}

/* Inicialização do compilador */
void Init()
{
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
char GetNum()
{
    char num;

    if (!isdigit(Look))
        Expected("Integer");
    num = Look;
    NextChar();

    return num;
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

/* Recebe uma classe de armazenamento C */
void GetClass()
{
    if (Look == 'a' || Look == 'x' || Look == 's') {
        CurrentClass = Look;
        NextChar();
    } else
        CurrentClass = 'a';
}

/* Recebe um tipo de dado C */
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

/* Analisa uma declaração de função */
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

/* Analisa uma declaração de variável */
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

/* Analisa uma declaração */
void TopDeclaration()
{
    char name;

    name = GetName();
    if (Look == '(')
        FunctionDeclaration(name);
    else
        DoData(name);
}

/* Programa principal */
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