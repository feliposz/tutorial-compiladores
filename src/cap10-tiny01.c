/*
Tiny 0.1 - Tokens de um caractere

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#define VARTABLE_SIZE 26
char VarTable[VARTABLE_SIZE]; /* Tabela de símbolos */

char Look; /* O caractere lido "antecipadamente" (lookahead) */
int LabelCount; /* Contador usado pelo gerador de rótulos */

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

    if (!isalpha(Look))
        Expected("Name");
    name = toupper(Look);
    NextChar();

    return name;
}

/* Recebe um número inteiro */
int GetNum()
{
    int num;

    num = 0;

    if (!isdigit(Look))
        Expected("Integer");

    while (isdigit(Look)) {
        num *= 10;
        num += Look - '0';
        NextChar();
    }

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

/* Reconhece um operador OU */
int IsOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* Reconhece operadores relacionais */
int IsRelOp(char c)
{
    return (c == '=' || c == '#' || c == '<' || c == '>');
}

/* Avisa a respeito de um identificador desconhecido */
void Undefined(char name)
{
    Abort("Undefined identifier %c\n", name);
}

/* Verifica se símbolo está na tabela */
int InTable(char name)
{
    return (VarTable[name - 'A'] != ' ');
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

/* Cabeçalho inicial para o montador */
void AsmHeader()
{
    printf("org 100h\n");
    printf("section .data\n");
}

/* Emite código para o prólogo de um programa */
void AsmProlog()
{
    printf("section .text\n");
    printf("_start:\n");
}

/* Emite código para o epílogo de um programa */
void AsmEpilog()
{
    EmitLn("MOV AX, 4C00h");
    EmitLn("INT 21h");
}

/* Zera o registrador primário */
void AsmClear()
{
    EmitLn("XOR AX, AX");
}

/* Negativa o registrador primário */
void AsmNegate()
{
    EmitLn("NEG AX");
}

/* Carrega uma constante numérica no registrador primário */
void AsmLoadConst(int i)
{
    EmitLn("MOV AX, %d", i);
}

/* Carrega uma variável no registrador primário */
void AsmLoadVar(char name)
{
    if (!InTable(name))
        Undefined(name);
    EmitLn("MOV AX, [%c]", name);
}

/* Armazena registrador primário em variável */
void AsmStore(char name)
{
    if (!InTable(name))
        Undefined(name);
    EmitLn("MOV [%c], AX", name);
}

/* Coloca registrador primário na pilha */
void AsmPush()
{
    EmitLn("PUSH AX");
}

/* Adiciona topo da pilha ao registrador primário */
void AsmPopAdd()
{
    EmitLn("POP BX");
    EmitLn("ADD AX, BX");
}

/* Subtrai o registrador primário do topo da pilha */
void AsmPopSub()
{
    EmitLn("POP BX");
    EmitLn("SUB AX, BX");
    EmitLn("NEG AX");
}

/* Multiplica o topo da pilha pelo registrador primário */
void AsmPopMul()
{
    EmitLn("POP BX");
    EmitLn("IMUL BX");
}

/* Divide o topo da pilha pelo registrador primário */
void AsmPopDiv()
{
    EmitLn("POP BX");
    EmitLn("XCHG AX, BX");
    EmitLn("CWD");
    EmitLn("IDIV BX");
}

/* Inverte registrador primário */
void AsmNot()
{
    EmitLn("NOT AX");
}

/* Aplica "E" binário ao topo da pilha com registrador primário */
void AsmPopAnd()
{
    EmitLn("POP BX");
    EmitLn("AND AX, BX");
}

/* Aplica "OU" binário ao topo da pilha com registrador primário */
void AsmPopOr()
{
    EmitLn("POP BX");
    EmitLn("OR AX, BX");
}

/* Aplica "OU-exclusivo" binário ao topo da pilha com registrador primário */
void AsmPopXor()
{
    EmitLn("POP BX");
    EmitLn("XOR AX, BX");
}

/* Compara topo da pilha com registrador primário */
void AsmPopCompare()
{
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
}

/* Altera registrador primário (e flags, indiretamente) conforme a comparação */
void AsmRelOp(char op)
{
    char *jump;
    int l1, l2;

    l1 = NewLabel();
    l2 = NewLabel();

    switch (op) {
        case '=': jump = "JE"; break;
        case '#': jump = "JNE"; break;
        case '<': jump = "JL"; break;
        case '>': jump = "JG"; break;
    }

    EmitLn("%s L%d", jump, l1);
    EmitLn("XOR AX, AX");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* Desvio incondicional */
void AsmBranch(int label)
{
    EmitLn("JMP L%d", label);
}

/* Desvio se falso (0) */
void AsmBranchFalse(int label)
{
    EmitLn("JZ L%d", label);
}

/* Alocação de memória para uma variável global */
void AllocVar(char name)
{
    int value = 0, signal = 1;

    if (InTable(name))
        Abort("Duplicate variable name: %c", name);
    else
        VarTable[name - 'A'] = 'v';

    if (Look == '=') {
        Match('=');
        if (Look == '-') {
            Match('-');
            signal = -1;
        }
        value = signal * GetNum();
    }    

    printf("%c\tdw %d\n", name, value);
}

/* Analisa uma lista de declaração de variáveis */
void Declaration()
{
    Match('v');
    for (;;) {
        AllocVar(GetName());
        if (Look != ',')
            break;
        Match(',');
    }
}

/* Analisa e traduz declarações globais */
void TopDeclarations()
{
    while (Look != 'b') {
        switch (Look) {
            case 'v':
                Declaration();
                break;
            default:
                Error("Unrecognized keyword.");
                Expected("BEGIN");
                break;
        }
    }
}

void BoolExpression(); /* Declaração adianta */

/* Analisa e traduz um fator matemático */
void Factor()
{
    if (Look == '(') {
        Match('(');
        BoolExpression();
        Match(')');
    } else if (isalpha(Look))
        AsmLoadVar(GetName());
    else
        AsmLoadConst(GetNum());
}

/* Analisa e traduz um fator negativo */
void NegFactor()
{
    Match('-');
    if (isdigit(Look))
        AsmLoadConst(-GetNum());
    else {
        Factor();
        AsmNegate();
    }
}

/* Analisa e traduz um fator inicial */
void FirstFactor()
{
    switch (Look) {
        case '+':
            Match('+');
            Factor();
            break;
        case '-':
            NegFactor();
            break;
        default:
            Factor();
            break;
    }
}

/* Reconhece e traduz uma multiplicação */
void Multiply()
{
    Match('*');
    Factor();
    AsmPopMul();
}

/* Reconhece e traduz uma divisão */
void Divide()
{
    Match('/');
    Factor();
    AsmPopDiv();
}

/* Código comum usado por "term" e "firstTerm" */
void TermCommon()
{
    while (IsMulOp(Look)) {
        AsmPush();
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

/* Analisa e traduz um termo matemático */
void Term()
{
    Factor();
    TermCommon();
}

/* Analisa e traduz um termo inicial */
void FirstTerm()
{
    FirstFactor();
    TermCommon();
}

/* Reconhece e traduz uma adição */
void Add()
{
    Match('+');
    Term();
    AsmPopAdd();
}

/* Reconhece e traduz uma subtração*/
void Subtract()
{
    Match('-');
    Term();
    AsmPopSub();
}

/* Analisa e traduz uma expressão matemática */
void Expression()
{
    FirstTerm();
    while (IsAddOp(Look)) {
        AsmPush();
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

/* Analisa e traduz uma relação */
void Relation()
{
    char op;

    Expression();
    if (IsRelOp(Look)) {
        op = Look;
        Match(op); /* Só para remover o operador do caminho */
        AsmPush();
        Expression();
        AsmPopCompare();
        AsmRelOp(op);
    }
}

/* Analisa e traduz um fator booleano com NOT inicial */
void NotFactor()
{
    if (Look == '!') {
        Match('!');
        Relation();
        AsmNot();
    } else
        Relation();
}

/* Analisa e traduz um termo booleano */
void BoolTerm()
{
    NotFactor();
    while (Look == '&') {
        AsmPush();
        Match('&');
        NotFactor();
        AsmPopAnd();
    }
}

/* Reconhece e traduz um operador OR */
void BoolOr()
{
    Match('|');
    BoolTerm();
    AsmPopOr();
}

/* Reconhece e traduz um operador XOR */
void BoolXor()
{
    Match('~');
    BoolTerm();
    AsmPopXor();
}

/* Analisa e traduz uma expressão booleana */
void BoolExpression()
{
    BoolTerm();
    while (IsOrOp(Look)) {
        AsmPush();
        switch (Look) {
          case '|':
              BoolOr();
              break;
          case '~':
              BoolXor();
              break;
        }
    }
}

/* Analisa e traduz um comando de atribuição */
void Assignment()
{
    char name;

    name = GetName();
    Match('=');
    BoolExpression();
    AsmStore(name);
}

void Block();

/* Analisa e traduz um comando IF */
void DoIf()
{
    int l1, l2;

    Match('i');
    BoolExpression();
    l1 = NewLabel();
    l2 = l1;
    AsmBranchFalse(l1);
    Block();
    if (Look == 'l') {
        Match('l');
        l2 = NewLabel();
        AsmBranch(l2);
        PostLabel(l1);
        Block();
    }
    PostLabel(l2);
    Match('e');
}

/* Analisa e traduz um comando WHILE */
void DoWhile()
{
    int l1, l2;

    Match('w');
    l1 = NewLabel();
    l2 = NewLabel();
    PostLabel(l1);
    BoolExpression();
    AsmBranchFalse(l2);
    Block();
    Match('e');
    AsmBranch(l1);
    PostLabel(l2);
}

/* Analisa e traduz um bloco de comandos */
void Block()
{
    int follow = 0;

    while (!follow) {
        switch (Look) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
            default:
                Assignment();
                break;
        }
    }
}

/* Analisa e traduz o bloco principal */
void MainBlock()
{
    Match('b');
    AsmProlog();
    Block();
    Match('e');
    AsmEpilog();
}

/* Analisa e traduz um programa completo */
void Program()
{
    Match('p');
    AsmHeader();
    TopDeclarations();
    MainBlock();
    Match('.');
}

/* Inicialização do compilador */
void Init()
{
    int i = 0;

    for (i = 0; i < VARTABLE_SIZE; i++)
        VarTable[i] = ' ';

    NextChar();
}

/* Programa principal */
int main()
{
    Init();
    Program();

    if (Look != '\n')
        Abort("Unexpected data after \'.\'");

    return 0;
}