/*
Uma visão do alto - Compilador "Pascal"

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

/* Funções de declaração pascal */

void Labels()
{
    Match('l');
}

void Constants()
{
    Match('c');
}

void Types()
{
    Match('t');
}

void Variables()
{
    Match('v');
}

void Procedure()
{
    Match('p');
}

void Function()
{
    Match('f');
}

/* Analisa e traduz declarações pascal */
void Declarations()
{
    int valid;

    do {
        valid = 1;
        switch (Look) {
            case 'l': Labels(); break;
            case 'c': Constants(); break;
            case 't': Types(); break;
            case 'v': Variables(); break;
            case 'p': Procedure(); break;
            case 'f': Function(); break;
            default: valid = 0; break;
        }
    } while (valid);
}

/* Analisa e traduz um bloco de comandos pascal */
void Statements()
{
    Match('b');
    while (Look != 'e')
        NextChar();
    Match('e');
}

/* Analisa e traduz um bloco Pascal */
void Block(char name)
{
    Declarations();
    printf("%c:\n", name);
    Statements();
}

/* Emite código para o prólogo de um programa */
void AsmProlog()
{
    EmitLn(".model small");
    EmitLn(".stack");
    EmitLn(".code");
    printf("PROG segment byte public\n");
    EmitLn("assume cs:PROG,ds:PROG,es:PROG,ss:PROG");
}

/* Emite código para o epílogo de um programa */
void AsmEpilog(char name)
{
    printf("exit_prog:\n");
    EmitLn("MOV AX, 4C00h  ; AH=4C (termina execucao do programa) AL=00 (saida ok)");
    EmitLn("INT 21h       ; chamada de sistema DOS");
    printf("PROG ends\n");
    EmitLn("end %c", name);
}

/* Analisa e traduz um programa Pascal */
void Program()
{
    char name;

    Match('p'); /* Trata do cabeçalho do programa */
    name = GetName();
    AsmProlog();
    Block(name);
    Match('.');
    AsmEpilog(name);
}

/* Programa principal */
int main()
{
    Init();
    Program();

    return 0;
}