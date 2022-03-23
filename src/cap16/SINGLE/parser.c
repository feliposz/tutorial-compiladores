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
    while (IsAddOp(look)) {
        switch (look) {
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
    while (IsMulOp(look)) {
        switch (look) {
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
    char sign = look;
    if (IsAddOp(look))
        NextChar();
    Term();
    if (sign == '-')
        AsmNegate();
}

/* analisa e traduz um fator matemático */
void Factor()
{
    if (look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (isdigit(look))
        AsmLoadConst(GetNum());
    else if (isalpha(look))
        AsmLoadVar(GetName());
    else
        Error("Unrecognized character: '%c'", look);
}

/* analisa e traduz um fator com NOT opcional */
void NotFactor()
{
    if (look == '!') {
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