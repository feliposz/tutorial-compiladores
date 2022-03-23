/*
Mais expressões: Variáveis, funções, tokens multi-caracter, espaços...

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#define MAXNAME 30
#define MAXNUM 5

char Look; /* O caracter lido "antecipadamente" (lookahead) */

/* Protótipos */
void Init();
void NextChar();
void Error(char *fmt, ...);
void Abort(char *fmt, ...);
void Expected(char *fmt, ...);
void SkipWhite();
void Match(char c);
void GetName(char *name);
void GetNum(char *num);
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

/* Programa principal */
int main()
{
    Init();
    Assignment();
    if (Look != '\n')
        Expected("NewLine");

    return 0;
}

/* Inicialização do compilador */
void Init()
{
    NextChar();
    SkipWhite();
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

/* Verifica se entrada combina com o esperado */
void Match(char c)
{
    if (Look != c)
        Expected("'%c'", c);
    NextChar();
    SkipWhite();
}

/* Recebe o nome de um identificador */
void GetName(char *name)
{
    int i;
    if (!isalpha(Look))
        Expected("Name");
    for (i = 0; isalnum(Look); i++) {
        if (i >= MAXNAME)
            Abort("Identifier too long!");
        name[i] = toupper(Look);
        NextChar();
    }
    name[i] = '\0';
    SkipWhite();
}

/* Recebe um número inteiro */
void GetNum(char *num)
{
    int i;
    if (!isdigit(Look))
        Expected("Integer");
    for (i = 0; isdigit(Look); i++) {
        if (i >= MAXNUM)
            Abort("Integer too long!");
        num[i] = Look;
        NextChar();
    }
    num[i] = '\0';
    SkipWhite();
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

/* Analisa e traduz um identificador */
void Ident()
{
    char name[MAXNAME+1];
    GetName(name);
    if (Look == '(') {
        Match('(');
        Match(')');
        EmitLn("CALL %s", name);
    } else
        EmitLn("MOV AX, [%s]", name);
}

/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name[MAXNAME+1];
    GetName(name);
    Match('=');
    Expression();
    EmitLn("MOV [%s], AX", name);
}

/* Analisa e traduz um fator */
void Factor()
{
    char num[MAXNUM+1];
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if(isalpha(Look)) {
        Ident();
    } else {
        GetNum(num);
        EmitLn("MOV AX, %s", num);
    }
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

/* Analisa e traduz um termo */
void Term()
{
    Factor();
    while (Look == '*' || Look == '/') {
        EmitLn("PUSH AX");
        switch (Look) {
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
        }
    }
}

/* Reconhece e traduz uma adição */
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

/* Reconhece e traduz uma expressão */
void Expression()
{
    if (IsAddOp(Look))
        EmitLn("XOR AX, AX");
    else
        Term();
    while (Look == '+' || Look == '-') {
        EmitLn("PUSH AX");
        switch (Look) {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
        }
    }
}

/* Reconhece operador aditivo */
int IsAddOp(char c)
{
    return (c == '+' || c == '-');
}
