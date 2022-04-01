/*
Estruturas de controle

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

char Look; /* O caractere lido "antecipadamente" (lookahead) */
int LabelCount; /* Contador usado pelo gerador de rótulos */

/* Protótipos */
void Init();
void NextChar();
void Error(char *fmt, ...);
void Abort(char *fmt, ...);
void Expected(char *fmt, ...);
void Match(char c);
char GetName();
char GetNum();
void EmitLn(char *fmt, ...);

int NewLabel();
void PostLabel(int lbl);

void Condition();
void Expression();

void Other();
void DoIf(int exitLabel);
void DoWhile();
void DoLoop();
void DoRepeat();
void DoFor();
void DoDo();
void DoBreak(int exitLabel);
void Block(int exitLabel);
void Program();

/* Programa principal */
int main()
{
    Init();
    Program();

    return 0;
}

/* Inicialização do compilador */
void Init()
{
    LabelCount = 0;
    NextChar();
}

/* Lê próximo caractere da entrada */
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

    if (!isupper(Look))
        Expected("Name");
    name = Look;
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

/* Gera um novo rótulo único */
int NewLabel()
{
    return LabelCount++;
}

/* Emite um rótulo */
void PostLabel(int lbl)
{
    printf("L%d:\n", lbl);
}

/* Analisa e traduz uma condição */
void Condition()
{
    EmitLn("; condition");
}

/* Analisa e traduz uma expressão */
void Expression()
{
    EmitLn("; expression");
}

/* Reconhece e traduz um comando qualquer */
void Other()
{
    EmitLn("; %c", GetName());
}

/* Analisa e traduz um comando IF */
void DoIf(int exitLabel)
{
    int l1, l2;

    Match('i');
    Condition();
    l1 = NewLabel();
    l2 = l1;
    EmitLn("JZ L%d", l1);
    Block(exitLabel);
    if (Look == 'l') {
        Match('l');
        l2 = NewLabel();
        EmitLn("JMP L%d", l2);
        PostLabel(l1);
        Block(exitLabel);
    }
    Match('e');
    PostLabel(l2);
}

/* Analisa e traduz um comando WHILE */
void DoWhile()
{
    int l1, l2;

    Match('w');
    l1 = NewLabel();
    l2 = NewLabel();
    PostLabel(l1);
    Condition();
    EmitLn("JZ L%d", l2);
    Block(l2);
    Match('e');
    EmitLn("JMP L%d", l1);
    PostLabel(l2);
}

/* Analisa e traduz um comando LOOP*/
void DoLoop()
{
    int l1, l2;

    Match('p');
    l1 = NewLabel();
    l2 = NewLabel();
    PostLabel(l1);
    Block(l2);
    Match('e');
    EmitLn("JMP L%d", l1);
    PostLabel(l2);
}

/* Analisa e traduz um REPEAT-UNTIL*/
void DoRepeat()
{
    int l1, l2;

    Match('r');
    l1 = NewLabel();
    l2 = NewLabel();
    PostLabel(l1);
    Block(l2);
    Match('u');
    Condition();
    EmitLn("JZ L%d", l1);
    PostLabel(l2);
}

/* Analisa e traduz um comando FOR*/
void DoFor()
{
    int l1, l2;
    char name;

    Match('f');
    l1 = NewLabel();
    l2 = NewLabel();
    name = GetName();
    Match('=');
    Expression();
    EmitLn("DEC AX");
    EmitLn("MOV [%c], AX", name);
    Expression();
    EmitLn("PUSH AX");
    PostLabel(l1);
    EmitLn("MOV AX, [%c]", name);
    EmitLn("INC AX");
    EmitLn("MOV [%c], AX", name);
    EmitLn("POP BX");
    EmitLn("PUSH BX");
    EmitLn("CMP AX, BX");
    EmitLn("JG L%d", l2);
    Block(l2);
    Match('e');
    EmitLn("JMP L%d", l1);
    PostLabel(l2);
    EmitLn("POP AX");
}

/* Analisa e traduz um comando DO */
void DoDo()
{
    int l1, l2;

    Match('d');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("MOV CX, AX");
    PostLabel(l1);
    EmitLn("PUSH CX");
    Block(l2);
    Match('e');
    EmitLn("POP CX");
    EmitLn("LOOP L%d", l1);
    EmitLn("PUSH CX");
    PostLabel(l2);
    EmitLn("POP CX");
}

/* Analisa e traduz um comando BREAK */
void DoBreak(int exitLabel)
{
    Match('b');
    if (exitLabel == -1)
        Abort("No loop to break from.");
    EmitLn("JMP L%d", exitLabel);
}

/* Analisa e traduz um bloco de comandos */
void Block(int exitLabel)
{
    int follow;

    follow = 0;

    while (!follow) {
        switch (Look) {
            case 'i':
                DoIf(exitLabel);
                break;
            case 'w':
                DoWhile();
                break;
            case 'p':
                DoLoop();
                break;
            case 'r':
                DoRepeat();
                break;
            case 'f':
                DoFor();
                break;
            case 'd':
                DoDo();
                break;
            case 'b':
                DoBreak(exitLabel);
                break;
            case 'e':
            case 'l':
            case 'u':
                follow = 1;
                break;
            default:
                Other();
                break;
        }
    }
}

/* Analisa e traduz um programa completo */
void Program()
{
    Block(-1);
    if (Look != 'e')
        Expected("End");
    EmitLn("; END");
}