/*
Mais expressões: Variáveis, funções, tokens de um caracter

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

/* protótipos */
void Init();
void NextChar();
void Error(char *fmt, ...);
void Abort(char *fmt, ...);
void Expected(char *fmt, ...);
void Match(char c);
char GetName();
char GetNum();
void EmitLn(char *fmt, ...);
void Ident();
void Assignment();
void Factor();
void Term();
void Expression();
void Add();
void Subtract();
void Multiply();
void Divide();
int IsAddOp(char c);

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    Assignment();
    if (look != '\n')
        Expected("NewLine");

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

/* analisa e traduz um identificador */
void Ident()
{
    char name;
    name = GetName();
    if (look == '(') {
        Match('(');
        Match(')');
        EmitLn("CALL %c", name);
    } else
        EmitLn("MOV AX, [%c]", name);
}

/* analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;
    name = GetName();
    Match('=');
    Expression();
    EmitLn("MOV [%c], AX", name);
}

/* analisa e traduz um fator */
void Factor()
{
    if (look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if(isalpha(look))
        Ident();
    else
        EmitLn("MOV AX, %c", GetNum());
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

/* analisa e traduz um termo */
void Term()
{
    Factor();
    while (look == '*' || look == '/') {
        EmitLn("PUSH AX");
        switch(look) {
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
        }
    }
}

/* reconhece e traduz uma adição */
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

/* reconhece e traduz uma expressão */
void Expression()
{
    if (IsAddOp(look))
        EmitLn("XOR AX, AX");
    else
        Term();
    while (look == '+' || look == '-') {
        EmitLn("PUSH AX");
        switch(look) {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
        }
    }
}

/* reconhece operador aditivo */
int IsAddOp(char c)
{
    return (c == '+' || c == '-');
}
