/*
Tiny 1.3 (Alternativa) - Trata comentários de linha iniciados por #

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
char *SymbolTable[SYMBOLTABLE_SIZE]; /* Tabela de símbolos */
char SymbolType[SYMBOLTABLE_SIZE]; /* Tabela de tipos de símbolos */
int SymbolCount; /* Número de entradas na tabela de símbolos */

char Look; /* O caractere lido "antecipadamente" (lookahead) */
int LabelCount; /* Contador usado pelo gerador de rótulos */

#define KEYWORDLIST_SIZE 9

/* Lista de palavras-chave */
char *KeywordList[KEYWORDLIST_SIZE] = {"IF", "ELSE", "ENDIF", "WHILE", "ENDWHILE",
                           "READ", "WRITE", "VAR", "END"};

/* Códigos de palavras-chave: a ordem deve obedecer a lista de palavras-chave */
char *KeywordCode = "ileweRWve";

#define MAXTOKEN 16
char TokenText[MAXTOKEN+1]; /* Texto do token atual */
char Token; /* Código do token atual */

/* Lê próximo caractere da entrada */
void NextChar()
{
    Look = getchar();
}

/* Pula um campo de comentário de uma só linha*/
void SkipComment()
{
    do {
        NextChar();
    } while (Look != '\n');
    NextChar();
}

/* Pula caracteres em branco */
void SkipWhite()
{
    while (isspace(Look) || Look == '#') {
        if (Look == '#')
            SkipComment();
        else
            NextChar();
    }
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

/* Avisa a respeito de um identificador desconhecido */
void Undefined(char *name)
{
    Abort("Undefined identifier %s\n", name);
}

/* Avisa a respeito de um identificador duplicado */
void Duplicate(char *name)
{
    Abort("Duplicated identifier %s\n", name);
}

/* Reporta um erro se Token NÃO for um identificador */
void CheckIdent()
{
    if (Token != 'x')
        Expected("Identifier");
}

/* Recebe o nome de um identificador ou palavra-chave */
void GetName()
{
    int i;

    SkipWhite();
    if (!isalpha(Look))
        Expected("Identifier or Keyword");
    for (i = 0; isalnum(Look) && i < MAXTOKEN; i++) {
        TokenText[i] = toupper(Look);
        NextChar();
    }
    TokenText[i] = '\0';
    Token = 'x';
}

/* Recebe um número inteiro */
void GetNum()
{
    int i;

    SkipWhite();
    if (!isdigit(Look))
        Expected("Integer");
    for (i = 0; isdigit(Look) && i < MAXTOKEN; i++) {
        TokenText[i] = Look;
        NextChar();
    }
    TokenText[i] = '\0';
    Token = '#';
}

/* Analisa e traduz um operador */
void GetOp()
{
    SkipWhite();
    Token = Look;
    TokenText[0] = Look;
    TokenText[1] = '\0';
    NextChar();
}

/* Pega o próximo Token de entrada */
void NextToken()
{
    SkipWhite();
    if (isalpha(Look))
        GetName();
    else if (isdigit(Look))
        GetNum();
    else
        GetOp();
}

/* Se a string de entrada estiver na tabela, devolve a posição ou -1 se não estiver */
int Lookup(char *s, char *list[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (strcmp(list[i], s) == 0)
            return i;
    }

    return -1;
}

/* Analisador léxico */
void Scan()
{
    int kw;

    if (Token == 'x') {
        kw = Lookup(TokenText, KeywordList, KEYWORDLIST_SIZE);
        if (kw >= 0)
            Token = KeywordCode[kw];
    }
}

/* Verifica se a string combina com o esperado */
void MatchString(char *s)
{
    if (strcmp(TokenText, s) != 0)
        Expected(s);
    NextToken();
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

/* Verifica se símbolo está na tabela */
int InTable(char *name)
{
    return (Lookup(name, SymbolTable, SymbolCount) >= 0);
}

/* Retorna o endereço do identificador na tabela de símbolos */
int Locate(char *name)
{
    return Lookup(name, SymbolTable, SymbolCount);
}

/* Reporta um erro se identificador NÃO constar na tabela de símbolos */
void CheckTable(char *name)
{
    if (!InTable(name))
        Undefined(name);
}

/* Reporta um erro se identificador JÁ constar na tabela de símbolos */
void CheckDuplicate(char *name)
{
    if (InTable(name))
        Duplicate(name);
}

/* Adiciona uma nova entrada à tabela de símbolos */
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
    printf("\n%%include \"tinyrtl_dos.inc\"\n");
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
void AsmLoadConst(char *num)
{
    EmitLn("MOV AX, %s", num);
}

/* Carrega uma variável no registrador primário */
void AsmLoadVar(char *name)
{
    EmitLn("MOV AX, [%s]", name);
}

/* Armazena registrador primário em variável */
void AsmStore(char *name)
{
    EmitLn("MOV [%s], AX", name);
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

/* Lê um valor a partir da entrada e armazena-o no registrador primário */
void AsmRead()
{
    EmitLn("CALL READ");
}

/* Mostra valor do registrador primário */
void AsmWrite()
{
    EmitLn("CALL WRITE");
}

/* Alocação de memória para uma variável global */
void AllocVar(char *name, int value)
{
    printf("%s\tdw %d\n", name, value);
}

/* Analisa uma lista de declaração de variáveis */
void Declaration()
{
    NextToken();
    CheckIdent();
    CheckDuplicate(TokenText);
    AddEntry(TokenText, 'v');
    AllocVar(TokenText, 0);
    NextToken();
}

/* Reconhece um ponto-e-vírgula opcional */
void Semicolon()
{
    if (Token == ';')
        MatchString(";");
}

/* Analisa e traduz declarações globais */
void TopDeclarations()
{
    Scan();
    while (Token == 'v') {
        do {
            Declaration();
        } while (Token == ',');
        Semicolon();
        Scan();
    }
}

void BoolExpression(); /* Declaração adianta */

/* Analisa e traduz um fator matemático */
void Factor()
{
    if (Token == '(') {
        NextToken();
        BoolExpression();
        MatchString(")");
    } else {
        if (Token == 'x') {
            CheckTable(TokenText);
            AsmLoadVar(TokenText);
        } else if (Token == '#')
            AsmLoadConst(TokenText);
        else
            Expected("Math Factor");
        NextToken();
    }
}

/* Reconhece e traduz uma multiplicação */
void Multiply()
{
    NextToken();
    Factor();
    AsmPopMul();
}

/* Reconhece e traduz uma divisão */
void Divide()
{
    NextToken();
    Factor();
    AsmPopDiv();
}

/* Analisa e traduz um termo matemático */
void Term()
{
    Factor();
    while (IsMulOp(Token)) {
        AsmPush();
        switch (Token) {
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
    NextToken();
    Term();
    AsmPopAdd();
}

/* Reconhece e traduz uma subtração*/
void Subtract()
{
    NextToken();
    Term();
    AsmPopSub();
}

/* Analisa e traduz uma expressão matemática */
void Expression()
{
    if (IsAddOp(Token))
        AsmClear();
    else
        Term();
    while (IsAddOp(Token)) {
        AsmPush();
        switch (Token) {
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
    if (IsRelOp(Token)) {
        op = Token;
        NextToken(); /* Só para remover o operador do caminho */
        if (op == '<') {
            if (Token == '>') { /* Trata operador <> */
                NextToken();
                op = '#';
            } else if (Token == '=') { /* Trata operador <= */
                NextToken();
                op = 'L';
            }
        } else if (op == '>' && Token == '=') { /* Trata operador >= */
            NextToken();
            op = 'G';
        }
        AsmPush();
        Expression();
        AsmPopCompare();
        AsmRelOp(op);
    }
}

/* Analisa e traduz um fator booleano com NOT inicial */
void NotFactor()
{
    if (Token == '!') {
        NextToken();
        Relation();
        AsmNot();
    } else
        Relation();
}

/* Analisa e traduz um termo booleano */
void BoolTerm()
{
    NotFactor();
    while (Token == '&') {
        NextToken();
        AsmPush();
        NotFactor();
        AsmPopAnd();
    }
}

/* Reconhece e traduz um operador OR */
void BoolOr()
{
    NextToken();
    BoolTerm();
    AsmPopOr();
}

/* Reconhece e traduz um operador XOR */
void BoolXor()
{
    NextToken();
    BoolTerm();
    AsmPopXor();
}

/* Analisa e traduz uma expressão booleana */
void BoolExpression()
{
    BoolTerm();
    while (IsOrOp(Token)) {
        AsmPush();
        switch (Token) {
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
    char name[MAXTOKEN+1];

    strcpy(name, TokenText);
    CheckTable(name);
    NextToken();
    MatchString("=");
    BoolExpression();
    AsmStore(name);
}

void Block();

/* Analisa e traduz um comando IF */
void DoIf()
{
    int l1, l2;

    NextToken();
    BoolExpression();
    l1 = NewLabel();
    l2 = l1;
    AsmBranchFalse(l1);
    Block();
    if (Token == 'l') {
        NextToken();
        l2 = NewLabel();
        AsmBranch(l2);
        PostLabel(l1);
        Block();
    }
    PostLabel(l2);
    MatchString("ENDIF");
}

/* Analisa e traduz um comando WHILE */
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

/* Processa um comando READ */
void DoRead()
{
    NextToken();
    MatchString("(");
    for (;;) {
        CheckIdent();
        CheckTable(TokenText);
        AsmRead();
        AsmStore(TokenText);
        NextToken();
        if (Token != ',')
            break;
        NextToken();
    }
    MatchString(")");
}

/* Processa um comando WRITE */
void DoWrite()
{
    NextToken();
    MatchString("(");
    for (;;) {
        Expression();
        AsmWrite();
        if (Token != ',')
            break;
        NextToken();
    }
    MatchString(")");
}

/* Remova o comentário para usar o estilo "Pascal" de ponto-e-vírgulas */
/* #define PASCAL_STYLE */

#ifdef PASCAL_STYLE

/* Analisa e traduz um único comando */
void Statement()
{
    Scan();
    switch (Token) {
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

/* Analisa e traduz um bloco de comandos estilo "Pascal" */
void Block()
{
    Statement();
    while (Token == ';') {
        NextToken();
        Statement();
    }
    Scan();
}

#else

/* Analisa e traduz um bloco de comandos estilo "C/Ada" */
void Block()
{
    int follow = 0;

    do {
        Scan();
        switch (Token) {
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

/* Inicialização do compilador */
void Init()
{
    SymbolCount = 0;

    NextChar();
    NextToken();
}

/* Programa principal */
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