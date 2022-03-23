/*
Tiny 1.3

O código abaixo foi escrito por Felipo Soranz e é uma adaptação
do código original em Pascal escrito por Jack W. Crenshaw em sua
série "Let's Build a Compiler".

Este código é de livre distribuição e uso.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#define SYMBOLTABLE_SIZE 1000
char *SymbolTable[SYMBOLTABLE_SIZE]; /* tabela de símbolos */
char SymbolType[SYMBOLTABLE_SIZE]; /* tabela de tipos de símbolos */
int SymbolCount; /* número de entradas na tabela de símbolos */

char look; /* O caracter lido "antecipadamente" (lookahead) */
int LabelCount; /* Contador usado pelo gerador de rótulos */

#define KEYWORDLIST_SIZE 9

/* lista de palavras-chave */
char *KeywordList[KEYWORDLIST_SIZE] = {"IF", "ELSE", "ENDIF", "WHILE", "ENDWHILE",
                           "READ", "WRITE", "VAR", "END"};

/* códigos de palavras-chave: a ordem deve obedecer a lista de palavras-chave */
char *KeywordCode = "ileweRWve";

#define MAXTOKEN 16
char value[MAXTOKEN+1]; /* texto do token atual */
char token; /* código do token atual */

/* lê próximo caracter da entrada */
void NextChar()
{
    look = getchar();
}

/* pula um campo de comentário */
void SkipComment()
{
    while (look != '}') {
        NextChar();
        if (look == '{') /* trata comentários aninhados */
            SkipComment();
    }
    NextChar();
}

/* pula caracteres em branco */
void SkipWhite()
{
    while (isspace(look) || look == '{') {
        if (look == '{')
            SkipComment();
        else
            NextChar();
    }
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

/* avisa a respeito de um identificador desconhecido */
void Undefined(char *name)
{
    Abort("Error: Undefined identifier %s\n", name);
}

/* avisa a respeito de um identificador duplicado */
void Duplicate(char *name)
{
    Abort("Error: Duplicated identifier %s\n", name);
}

/* reporta um erro se token NÃO for um identificador */
void CheckIdent()
{
    if (token != 'x')
        Expected("Identifier");
}

/* recebe o nome de um identificador ou palavra-chave */
void GetName()
{
    int i;

    SkipWhite();
    if (!isalpha(look))
        Expected("Identifier or Keyword");
    for (i = 0; isalnum(look) && i < MAXTOKEN; i++) {
        value[i] = toupper(look);
        NextChar();
    }
    value[i] = '\0';
    token = 'x';
}

/* recebe um número inteiro */
void GetNum()
{
    int i;

    SkipWhite();
    if (!isdigit(look))
        Expected("Integer");
    for (i = 0; isdigit(look) && i < MAXTOKEN; i++) {
        value[i] = look;
        NextChar();
    }
    value[i] = '\0';
    token = '#';
}

/* analisa e traduz um operador */
void GetOp()
{
    SkipWhite();
    token = look;
    value[0] = look;
    value[1] = '\0';
    NextChar();
}

/* pega o próximo token de entrada */
void NextToken()
{
    SkipWhite();
    if (isalpha(look))
        GetName();
    else if (isdigit(look))
        GetNum();
    else
        GetOp();
}

/* se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int Lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}

/* analisador léxico. analisa identificador ou palavra-chave */
void Scan()
{
    int kw;

    if (token == 'x') {
        kw = Lookup(value, KeywordList, KEYWORDLIST_SIZE);
        if (kw >= 0)
            token = KeywordCode[kw];
    }
}

/* verifica se a string combina com o esperado */
void MatchString(char *s)
{
    if (strcmp(value, s) != 0)
        Expected(s);
    NextToken();
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

/* reconhece um operador OU */
int IsOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* reconhece operadores relacionais */
int IsRelOp(char c)
{
    return (c == '=' || c == '#' || c == '<' || c == '>');
}

/* verifica se símbolo está na tabela */
int InTable(char *name)
{
    return (Lookup(name, SymbolTable, SymbolCount) >= 0);
}

/* retorna o endereço do identificador na tabela de símbolos */
int Locate(char *name)
{
    return Lookup(name, SymbolTable, SymbolCount);
}

/* reporta um erro se identificador NÃO constar na tabela de símbolos */
void CheckTable(char *name)
{
    if (!InTable(name))
        Undefined(name);
}

/* reporta um erro se identificador JÁ constar na tabela de símbolos */
void CheckDuplicate(char *name)
{
    if (InTable(name))
        Duplicate(name);
}

/* adiciona uma nova entrada à tabela de símbolos */
void AddEntry(char *name, char type)
{
    char *newSymbol;

    CheckDuplicate(name);

    if (SymbolCount >= SYMBOLTABLE_SIZE) {
        Abort("Symbol table full!");
    }

    newSymbol = (char *) malloc(sizeof(char) * (strlen(name) + 1));
    if (newSymbol == NULL)
        Abort("Out of memory!");

    strcpy(newSymbol, name);

    SymbolTable[SymbolCount] = newSymbol;
    SymbolType[SymbolCount] = type;
    SymbolCount++;
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

/* cabeçalho inicial para o montador */
void AsmHeader()
{
    EmitLn(".model small");
    EmitLn(".stack");
    EmitLn(".code");
    printf("extrn READ:near, WRITE:near\n");
    printf("PROG segment byte public\n");
    EmitLn("assume cs:PROG,ds:PROG,es:PROG,ss:PROG");
}

/* emite código para o prólogo de um programa */
void AsmProlog()
{
    printf("MAIN:\n");
    EmitLn("MOV AX, PROG");
    EmitLn("MOV DS, AX");
    EmitLn("MOV ES, AX");
}

/* emite código para o epílogo de um programa */
void AsmEpilog()
{
    EmitLn("MOV AX, 4C00h");
    EmitLn("INT 21h");
    printf("PROG ends\n");
    EmitLn("end MAIN");
}

/* zera o registrador primário */
void AsmClear()
{
    EmitLn("XOR AX, AX");
}

/* negativa o registrador primário */
void AsmNegate()
{
    EmitLn("NEG AX");
}

/* carrega uma constante numérica no registrador primário */
void AsmLoadConst(char *num)
{
    EmitLn("MOV AX, %s", num);
}

/* carrega uma variável no registrador primário */
void AsmLoadVar(char *name)
{
    EmitLn("MOV AX, WORD PTR %s", name);
}

/* armazena registrador primário em variável */
void AsmStore(char *name)
{
    EmitLn("MOV WORD PTR %s, AX", name);
}

/* coloca registrador primário na pilha */
void AsmPush()
{
    EmitLn("PUSH AX");
}

/* adiciona o topo da pilha ao registrador primário */
void AsmPopAdd()
{
    EmitLn("POP BX");
    EmitLn("ADD AX, BX");
}

/* subtrai o registrador primário do topo da pilha */
void AsmPopSub()
{
    EmitLn("POP BX");
    EmitLn("SUB AX, BX");
    EmitLn("NEG AX");
}

/* multiplica o topo da pilha pelo registrador primário */
void AsmPopMul()
{
    EmitLn("POP BX");
    EmitLn("IMUL BX");
}

/* divide o topo da pilha pelo registrador primário */
void AsmPopDiv()
{
    EmitLn("POP BX");
    EmitLn("XCHG AX, BX");
    EmitLn("CWD");
    EmitLn("IDIV BX");
}

/* inverte registrador primário */
void AsmNot()
{
    EmitLn("NOT AX");
}

/* aplica "E" binário ao topo da pilha com registrador primário */
void AsmPopAnd()
{
    EmitLn("POP BX");
    EmitLn("AND AX, BX");
}

/* aplica "OU" binário ao topo da pilha com registrador primário */
void AsmPopOr()
{
    EmitLn("POP BX");
    EmitLn("OR AX, BX");
}

/* aplica "OU-exclusivo" binário ao topo da pilha com registrador primário */
void AsmPopXor()
{
    EmitLn("POP BX");
    EmitLn("XOR AX, BX");
}

/* compara topo da pilha com registrador primário */
void AsmPopCompare()
{
    EmitLn("POP BX");
    EmitLn("CMP BX, AX");
}

/* altera registrador primário (e flags, indiretamente) conforme a comparação */
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
        case 'L': jump = "JLE"; break;
        case 'G': jump = "JGE"; break;
    }

    EmitLn("%s L%d", jump, l1);
    EmitLn("XOR AX, AX");
    EmitLn("JMP L%d", l2);
    PostLabel(l1);
    EmitLn("MOV AX, -1");
    PostLabel(l2);
}

/* desvio incondicional */
void AsmBranch(int label)
{
    EmitLn("JMP L%d", label);
}

/* desvio se falso (0) */
void AsmBranchFalse(int label)
{
    EmitLn("JZ L%d", label);
}

/* lê um valor a partir da entrada e armazena-o no registrador primário */
void AsmRead()
{
    EmitLn("CALL READ");
}

/* mostra valor do registrador primário */
void AsmWrite()
{
    EmitLn("CALL WRITE");
}

/* alocação de memória para uma variável global */
void AllocVar(char *name, int value)
{
    printf("%s:\tdw %d\n", name, value);
}

/* analisa uma lista de declaração de variáveis */
void Declaration()
{
    NextToken();
    CheckIdent();
    CheckDuplicate(value);
    AddEntry(value, 'v');
    AllocVar(value, 0);
    NextToken();
}

/* reconhece um ponto-e-vírgula opcional */
void Semicolon()
{
    if (token == ';')
        MatchString(";");
}

/* analisa e traduz declarações globais */
void TopDeclarations()
{
    Scan();
    while (token == 'v') {
        do {
            Declaration();
        } while (token == ',');
        Semicolon();
        Scan();
    }
}

void BoolExpression(); /* declaração adianta */

/* analisa e traduz um fator matemático */
void Factor()
{
    if (token == '(') {
        NextToken();
        BoolExpression();
        MatchString(")");
    } else {
        if (token == 'x') {
            CheckTable(value);
            AsmLoadVar(value);
        } else if (token == '#')
            AsmLoadConst(value);
        else
            Expected("Math Factor");
        NextToken();
    }
}

/* reconhece e traduz uma multiplicação */
void Multiply()
{
    NextToken();
    Factor();
    AsmPopMul();
}

/* reconhece e traduz uma divisão */
void Divide()
{
    NextToken();
    Factor();
    AsmPopDiv();
}

/* analisa e traduz um termo matemático */
void Term()
{
    Factor();
    while (IsMulOp(token)) {
        AsmPush();
        switch (token) {
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
    NextToken();
    Term();
    AsmPopAdd();
}

/* reconhece e traduz uma subtração*/
void Subtract()
{
    NextToken();
    Term();
    AsmPopSub();
}

/* analisa e traduz uma expressão matemática */
void Expression()
{
    if (IsAddOp(token))
        AsmClear();
    else
        Term();
    while (IsAddOp(token)) {
        AsmPush();
        switch (token) {
            case '+':
                Add();
                break;
            case '-':
                Subtract();
                break;
        }
    }
}

/* analisa e traduz uma relação */
void Relation()
{
    char op;

    Expression();
    if (IsRelOp(token)) {
        op = token;
        NextToken(); /* só para remover o operador do caminho */
        if (op == '<') {
            if (token == '>') { /* trata operador <> */
                NextToken();
                op = '#';
            } else if (token == '=') { /* trata operador <= */
                NextToken();
                op = 'L';
            }
        } else if (op == '>' && token == '=') { /* trata operador >= */
            NextToken();
            op = 'G';
        }
        AsmPush();
        Expression();
        AsmPopCompare();
        AsmRelOp(op);
    }
}

/* analisa e traduz um fator booleano com NOT inicial */
void NotFactor()
{
    if (token == '!') {
        NextToken();
        Relation();
        AsmNot();
    } else
        Relation();
}

/* analisa e traduz um termo booleano */
void BoolTerm()
{
    NotFactor();
    while (token == '&') {
        NextToken();
        AsmPush();
        NotFactor();
        AsmPopAnd();
    }
}

/* reconhece e traduz um "OR" */
void BoolOr()
{
    NextToken();
    BoolTerm();
    AsmPopOr();
}

/* reconhece e traduz um "xor" */
void BoolXor()
{
    NextToken();
    BoolTerm();
    AsmPopXor();
}

/* analisa e traduz uma expressão booleana */
void BoolExpression()
{
    BoolTerm();
    while (IsOrOp(token)) {
        AsmPush();
        switch (token) {
          case '|':
              BoolOr();
              break;
          case '~':
              BoolXor();
              break;
        }
    }
}

/* analisa e traduz um comando de atribuição */
void Assignment()
{
    char name[MAXTOKEN+1];

    strcpy(name, value);
    CheckTable(name);
    NextToken();
    MatchString("=");
    BoolExpression();
    AsmStore(name);
}

void Block();

/* analisa e traduz um comando IF */
void DoIf()
{
    int l1, l2;

    NextToken();
    BoolExpression();
    l1 = NewLabel();
    l2 = l1;
    AsmBranchFalse(l1);
    Block();
    if (token == 'l') {
        NextToken();
        l2 = NewLabel();
        AsmBranch(l2);
        PostLabel(l1);
        Block();
    }
    PostLabel(l2);
    MatchString("ENDIF");
}

/* analisa e traduz um comando WHILE */
void DoWhile()
{
    int l1, l2;

    NextToken();
    l1 = NewLabel();
    l2 = NewLabel();
    PostLabel(l1);
    BoolExpression();
    AsmBranchFalse(l2);
    Block();
    MatchString("ENDWHILE");
    AsmBranch(l1);
    PostLabel(l2);
}

/* processa um comando READ */
void DoRead()
{
    NextToken();
    MatchString("(");
    for (;;) {
        CheckIdent();
        CheckTable(value);
        AsmRead();
        AsmStore(value);
        NextToken();
        if (token != ',')
            break;
        NextToken();
    }
    MatchString(")");
}

/* processa um comando WRITE */
void DoWrite()
{
    NextToken();
    MatchString("(");
    for (;;) {
        Expression();
        AsmWrite();
        if (token != ',')
            break;
        NextToken();
    }
    MatchString(")");
}

/* Remova o comentário para usar o estilo "Pascal" de ponto-e-vírgulas */
/* #define PASCAL_STYLE */

#ifdef PASCAL_STYLE

/* analisa e traduz um único comando */
void Statement()
{
    Scan();
    switch (token) {
        case 'i':
            DoIf();
            break;
        case 'w':
            DoWhile();
            break;
        case 'R':
            DoRead();
            break;
        case 'W':
            DoWrite();
            break;
        case 'x':
            Assignment();
            break;
    }
}

/* analiza e traduz um bloco de comandos estilo "Pascal" */
void Block()
{
    Statement();
    while (token == ';') {
        NextToken();
        Statement();
    }
    Scan();
}

#else

/* analisa e traduz um bloco de comandos estilo "C/Ada" */
void Block()
{
    int follow = 0;

    do {
        Scan();
        switch (token) {
            case 'i':
                DoIf();
                break;
            case 'w':
                DoWhile();
                break;
            case 'R':
                DoRead();
                break;
            case 'W':
                DoWrite();
                break;
            case 'x':
                Assignment();
                break;
            case 'e':
            case 'l':
                follow = 1;
                break;
        }
        if (!follow)
            Semicolon();
    } while (!follow);
}

#endif

/* inicialização do compilador */
void Init()
{
    SymbolCount = 0;

    NextChar();
    NextToken();
}

/* PROGRAMA PRINCIPAL */
int main()
{
    Init();

    MatchString("PROGRAM");
    Semicolon();
    AsmHeader();
    TopDeclarations();
    MatchString("BEGIN");
    AsmProlog();
    Block();
    MatchString("END");
    AsmEpilog();

    return 0;
}