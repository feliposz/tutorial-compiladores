#include "output.h"
#include "codegen.h"

/* Carrega uma constante no registrador primário */
void AsmLoadConst(char *s)
{
    EmitLn("MOV AX, %s", s);
}

/* Carrega uma variável no registrador primário */
void AsmLoadVar(char *s)
{
    EmitLn("MOV AX, %s", s);
}

/* Armazena valor do registrador primário em variável */
void AsmStoreVar(char *s)
{
    EmitLn("MOV %s, AX", s);
}

/* Inverte sinal de registrador primário */
void AsmNegate()
{
    EmitLn("NEG AX");
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

/* Subtrai do topo da pilha o registrador primário */
void AsmPopSub()
{
    EmitLn("POP BX");
    EmitLn("SUB AX, BX");
    AsmNegate();
}

/* Multiplica topo da pilha e registrador primário */
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

/* Aplica OU com topo da pilha a registrador primário */
void AsmPopOr()
{
    EmitLn("POP BX");
    EmitLn("OR AX, BX");
}

/* Aplica OU-exclusivo com topo da pilha a registrador primário */
void AsmPopXor()
{
    EmitLn("POP BX");
    EmitLn("XOR AX, BX");
}

/* Aplica AND com topo da pilha e registrador primário */
void AsmPopAnd()
{
    EmitLn("POP BX");
    EmitLn("AND AX, BX");
}

/* Aplica NOT ao registrador primário */
void AsmNot()
{
    EmitLn("NOT AX");
}