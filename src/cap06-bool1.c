/*
Expressões Booleanas - Parte 1 (até antes de "Adicionando estruturas de controle")

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
int LabelCount; /* Contador usado pelo gerador de rótulos */

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

int IsAddOp(char c);
int IsMulOp(char c);
int IsOrOp(char c);
int IsRelOp(char c);
int IsBoolean(char c);
int GetBoolean();

int NewLabel();
void PostLabel(int lbl);

void BoolFactor();
void NotFactor();
void BoolTerm();
void BoolOr();
void BoolXor();
void BoolExpression();
void Equals();
void NotEquals();
void Greater();
void Less();
void Relation();
void Ident();
void Factor();
void SignedFactor();
void Multiply();
void Divide();
void Term();
void Add();
void Subtract();
void Expression();

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();
    BoolExpression();
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
    Look = getchar();
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

/* reconhece operador relacional */
int IsRelOp(char c)
{
    return (c == '=' || c == '#' || c == '>' || c == '<');
}

/* reconhece uma literal Booleana */
int IsBoolean(char c)
{
    return (c == 'T' || c == 'F');
}

/* reconhece um operador OU */
int IsOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* recebe uma literal Booleana */
int GetBoolean()
{
    int boolean;

    if (!IsBoolean(Look))
        Expected("Boolean Literal");
    boolean = (Look == 'T');
    NextChar();

    return boolean;
}

/* gera um novo rótulo único */
int NewLabel()
{
    return LabelCount++;
}

/* emite um rótulo */
void PostLabel(int lbl)
{
    printf("L%d:\n", lbl);
}

/* analisa e traduz um fator booleano */
void BoolFactor()
{
    if (IsBoolean(Look)) {
        if (GetBoolean())
            EmitLn("MOV AX, -1");
        else
            EmitLn("MOV AX, 0");
    } else
        Relation();
}

/* analisa e traduz um fator booleno com NOT opcional */
void NotFactor()
{
    if (Look == '!') {
        Match('!');
        BoolFactor();
        EmitLn("NOT AX");
    } else
        BoolFactor();
}

/* analisa e traduz um termo booleano*/
void BoolTerm()
{
    NotFactor();
    while (Look == '&') {
        EmitLn("PUSH AX");
        Match('&');
        NotFactor();
        EmitLn("POP BX");
        EmitLn("AND AX, BX");
    }
}

/* reconhece e traduz um operador OR */
void BoolOr()
{
    Match('|');
    BoolTerm();
    EmitLn("POP BX");
    EmitLn("OR AX, BX");
}

/* reconhece e traduz um operador XOR */
void BoolXor()
{
    Match('~');
    BoolTerm();
    EmitLn("POP BX");
    EmitLn("XOR AX, BX");
}

/* analisa e traduz uma expressão booleana */
void BoolExpression()
{
    BoolTerm();
    while (IsOrOp(Look)) {
        EmitLn("PUSH AX");
        switch (Look) {
          case '|':
              BoolOr();
              break;
          case '~' :
              BoolXor();
              break;
        }
    }
}

/* reconhece e traduz um operador de igualdade */
void Equals()
{
    int l1, l2;

    Match('=');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
    EmitLn("JE L%d", l1);
    EmitLn("MOV AX, 0");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* reconhece e traduz um operador de não-igualdade */
void NotEquals()
{
    int l1, l2;

    Match('#');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
    EmitLn("JNE L%d", l1);
    EmitLn("MOV AX, 0");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* reconhece e traduz um operador de maior que */
void Greater()
{
    int l1, l2;

    Match('>');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
    EmitLn("JG L%d", l1);
    EmitLn("MOV AX, 0");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* reconhece e traduz um operador de menor que */
void Less()
{
    int l1, l2;

    Match('<');
    l1 = NewLabel();
    l2 = NewLabel();
    Expression();
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
    EmitLn("JL L%d", l1);
    EmitLn("MOV AX, 0");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* analisa e traduz uma relação */
void Relation()
{
    Expression();
    if (IsRelOp(Look)) {
        EmitLn("PUSH AX");
        switch (Look) {
            case '=':
                Equals();
                break;
            case '#':
                NotEquals();
                break;
            case '>':
                Greater();
                break;
            case '<':
                Less();
                break;
        }
    }
}

/* analisa e traduz um identificador */
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

/* analisa e traduz um fator matemático */
void Factor()
{
    if (Look == '(') {
        Match('(');
        BoolExpression();
        Match(')');
    } else if(isalpha(Look))
        Ident();
    else
        EmitLn("MOV AX, %c", GetNum());
}

/* analisa e traduz um fator com sinal opcional */
void SignedFactor()
{
    int minusSign = (Look == '-');
    if (IsAddOp(Look))
    {
        NextChar();
    }
    Factor();
    if (minusSign)
        EmitLn("NEG AX");
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

/* analisa e traduz um termo matemático */
void Term()
{
    SignedFactor();
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

/* reconhece e traduz uma soma */
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

/* analisa e traduz uma expressão matemática */
void Expression()
{
    Term();
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
