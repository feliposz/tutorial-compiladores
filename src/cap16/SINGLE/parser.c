#include <ctype.h>
#include "input.h"
#include "scanner.h"
#include "codegen.h"
#include "errors.h"
#include "parser.h"

/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;

	name = GetName();
    Match('=');
    Expression();
    AsmStoreVar(name);
}

/* Analisa e traduz uma expressão */
void Expression()
{
    SignedTerm();
    while (IsAddOp(Look)) {
        switch (Look) {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
            case '|':
                BoolOr();
                break;
            case '~':
                BoolXor();
                break;
        }
    }
}

/* Analisa e traduz um termo */
void Term()
{
    NotFactor();
    while (IsMulOp(Look)) {
        switch (Look) {
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
            case '&':
                BoolAnd();
                break;
        }
    }
}

/* Analisa e traduz um termo com um sinal opcional */
void SignedTerm()
{
    char sign = Look;
    if (IsAddOp(Look))
        NextChar();
    Term();
    if (sign == '-')
        AsmNegate();
}

/* Analisa e traduz um fator matemático */
void Factor()
{
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (isdigit(Look))
        AsmLoadConst(GetNum());
    else if (isalpha(Look))
        AsmLoadVar(GetName());
    else
        Error("Unrecognized character: '%c'", Look);
}

/* Analisa e traduz um fator com NOT opcional */
void NotFactor()
{
    if (Look == '!') {
        Match('!');
        Factor();
        AsmNot();
    } else
        Factor();
}

/* Analisa e traduz uma operação de soma */
void Add()
{
    Match('+');
    AsmPush();
    Term();
    AsmPopAdd();
}

/* Analisa e traduz uma operação de subtração */
void Subtract()
{
    Match('-');
    AsmPush();
    Term();
    AsmPopSub();
}

/* Analisa e traduz uma operação de multiplicação */
void Multiply()
{
    Match('*');
    AsmPush();
    NotFactor();
    AsmPopMul();
}

/* Analisa e traduz uma operação de divisão */
void Divide()
{
    Match('/');
    AsmPush();
    NotFactor();
    AsmPopDiv();
}

/* Analisa e traduz uma operação OU booleana */
void BoolOr()
{
    Match('|');
    AsmPush();
    Term();
    AsmPopOr();
}

/* Analisa e traduz uma operação OU-exclusivo booleana */
void BoolXor()
{
    Match('~');
    AsmPush();
    Term();
    AsmPopXor();
}

/* Analisa e traduz uma operação AND */
void BoolAnd()
{
    Match('&');
    AsmPush();
    NotFactor();
    AsmPopAnd();
}