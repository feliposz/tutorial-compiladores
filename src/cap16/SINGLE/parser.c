#include <ctype.h>
#include "input.h"
#include "scanner.h"
#include "codegen.h"
#include "errors.h"
#include "parser.h"

/* analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;

	name = GetName();
    Match('=');
    Expression();
    AsmStoreVar(name);
}

/* analisa e traduz uma expressão */
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

/* analisa e traduz um termo */
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

/* analisa e traduz um termo com um sinal opcional */
void SignedTerm()
{
    char sign = Look;
    if (IsAddOp(Look))
        NextChar();
    Term();
    if (sign == '-')
        AsmNegate();
}

/* analisa e traduz um fator matemático */
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

/* analisa e traduz um fator com NOT opcional */
void NotFactor()
{
    if (Look == '!') {
        Match('!');
        Factor();
        AsmNot();
    } else
        Factor();
}

/* analisa e traduz uma operação de soma */
void Add()
{
    Match('+');
    AsmPush();
    Term();
    AsmPopAdd();
}

/* analisa e traduz uma operação de subtração */
void Subtract()
{
    Match('-');
    AsmPush();
    Term();
    AsmPopSub();
}

/* analisa e traduz uma operação de multiplicação */
void Multiply()
{
    Match('*');
    AsmPush();
    NotFactor();
    AsmPopMul();
}

/* analisa e traduz uma operação de divisão */
void Divide()
{
    Match('/');
    AsmPush();
    NotFactor();
    AsmPopDiv();
}

/* analisa e traduz uma operação OU booleana */
void BoolOr()
{
    Match('|');
    AsmPush();
    Term();
    AsmPopOr();
}

/* analisa e traduz uma operação OU-exclusivo booleana */
void BoolXor()
{
    Match('~');
    AsmPush();
    Term();
    AsmPopXor();
}

/* analisa e traduz uma operação AND */
void BoolAnd()
{
    Match('&');
    AsmPush();
    NotFactor();
    AsmPopAnd();
}