#include "output.h"
#include "codegen.h"

/* carrega uma constante no registrador primário */
void AsmLoadConst(char c)
{
    EmitLn("MOV AX, %c", c);
}

/* carrega uma variável no registrador primário */
void AsmLoadVar(char c)
{
    EmitLn("MOV AX, %c", c);
}

/* armazena valor do registrador primário em variável */
void AsmStoreVar(char c)
{
    EmitLn("MOV %c, AX", c);
}

/* inverte sinal de registrador primário */
void AsmNegate()
{
    EmitLn("NEG AX");
}

/* coloca registrador primário na pilha */
void AsmPush()
{
    EmitLn("PUSH AX");
}

/* adiciona topo da pilha ao registrador primário */
void AsmPopAdd()
{
    EmitLn("POP BX");
    EmitLn("ADD AX, BX");
}

/* subtrai do topo da pilha o registrador primário */
void AsmPopSub()
{
    EmitLn("POP BX");
    EmitLn("SUB AX, BX");
    AsmNegate();
}

/* multiplica topo da pilha e registrador primário */
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

/* aplica OU com topo da pilha a registrador primário */
void AsmPopOr()
{
    EmitLn("POP BX");
    EmitLn("OR AX, BX");
}

/* aplica OU-exclusivo com topo da pilha a registrador primário */
void AsmPopXor()
{
    EmitLn("POP BX");
    EmitLn("XOR AX, BX");
}

/* aplica AND com topo da pilha e registrador primário */
void AsmPopAnd()
{
    EmitLn("POP BX");
    EmitLn("AND AX, BX");
}

/* aplica NOT ao registrador primário */
void AsmNot()
{
    EmitLn("NOT AX");
}