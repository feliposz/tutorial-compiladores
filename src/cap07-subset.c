/*
Juntando os analisadores: Primeiro passo - Sub-conjunto

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
void NewLine();
void SkipWhite();

int IsAddOp(char c);
int IsMulOp(char c);

int NewLabel();
void PostLabel(int lbl);

void Ident();
void Factor();
void SignedFactor();
void Add();
void Subtract();
void Multiply();
void Divide();
void TermCommon();
void FirstTerm();
void Term();
void Expression();
void Assignment();
void Condition();

void DoIf();
void Block();
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
    SkipWhite();
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
    SkipWhite();
}

/* Recebe o nome de um identificador */
char GetName()
{
    char name;

    while (Look == '\n')
        NewLine();
    if (!isalpha(Look))
        Expected("Name");
    name = toupper(Look);
    NextChar();
    SkipWhite();

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
    SkipWhite();

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

/* Reconhece uma linha em branco */
void NewLine()
{
    if (Look == '\n')
        NextChar();
}

/* Pula caracteres de espaço */
void SkipWhite()
{
    while (Look == ' ' || Look == '\t')
        NextChar();
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

/* Analisa e traduz um identificador */
void Ident()
{
    char name;

    name = GetName();
    if (Look == '(') {
        Match('(');
        Match(')');
        EmitLn("CALL %c", name);
    } else
        EmitLn("MOV AX, [%c]", name);
}

/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    Expression();
    EmitLn("MOV [%c], AX", name);
}

/* Analisa e traduz um fator matemático */
void Factor()
{
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if(isalpha(Look))
        Ident();
    else
        EmitLn("MOV AX, %c", GetNum());
}

/* Analisa e traduz um fator com sinal opcional */
void SignedFactor()
{
    int minusSign = (Look == '-');
    if (IsAddOp(Look))
    {
        NextChar();
        SkipWhite();
    }
    Factor();
    if (minusSign)
        EmitLn("NEG AX");
}

/* Reconhece e traduz uma soma */
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

/* Código comum usado por "term" e "firstTerm" */
void TermCommon()
{
    while (IsMulOp(Look)) {
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

/* Analisa e traduz um termo matemático no início de uma expressão */
void FirstTerm()
{
    SignedFactor();
    TermCommon();
}

/* Analisa e traduz um termo matemático */
void Term()
{
    Factor();
    TermCommon();
}

/* Analisa e traduz uma expressão matemática */
void Expression()
{
    FirstTerm();
    while (IsAddOp(Look)) {
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

/* Analisa e traduz uma condição */
void Condition()
{
    EmitLn("; condition");
}

void DoIf()
{
    int l1, l2;

    Match('i');
    Condition();
    l1 = NewLabel();
    l2 = l1;
    EmitLn("JZ L%d", l1);
    Block();
    if (Look == 'l') {
        Match('l');
        l2 = NewLabel();
        EmitLn("JMP L%d", l2);
        PostLabel(l1);
        Block();
    }
    Match('e');
    PostLabel(l2);
}

/* Analisa e traduz um bloco de comandos */
void Block()
{
    int follow;

    follow = 0;

    while (!follow) {
        switch (Look) {
            case 'i':
                DoIf();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            case '\n':
                NewLine();
                break;
            default:
                Assignment(); 
                break;
        }
    }
}

/* Analisa e traduz um programa completo */
void Program()
{
    Block();
    if (Look != 'e')
        Expected("End");
    EmitLn("; END");
}