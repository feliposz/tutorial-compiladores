#include "output.h"
#include "codegen.h"

/* carrega uma constante no registrador primário */
void asmLoadConstant(char *s)
{
    emit("MOV AX, %s", s);
}

/* carrega uma variável no registrador primário */
void asmLoadVariable(char *s)
{
    emit("MOV AX, %s", s);
}

/* armazena valor do registrador primário em variável */
void asmStoreVariable(char *s)
{
    emit("MOV %s, AX", s);
}

/* inverte sinal de registrador primário */
void asmNegate()
{
    emit("NEG AX");
}

/* coloca registrador primário na pilha */
void asmPush()
{
    emit("PUSH AX");
}

/* adiciona topo da pilha ao registrador primário */
void asmPopAdd()
{
    emit("POP BX");
    emit("ADD AX, BX");
}

/* subtrai do topo da pilha o registrador primário */
void asmPopSub()
{
    emit("POP BX");
    emit("SUB AX, BX");
    asmNegate();
}

/* multiplica topo da pilha e registrador primário */
void asmPopMul()
{
    emit("POP BX");
    emit("IMUL BX");
}

/* divide o topo da pilha pelo registrador primário */
void asmPopDiv()
{
    emit("POP BX");
    emit("XCHG AX, BX");
    emit("CWD");
    emit("IDIV BX");
}

/* aplica OU com topo da pilha a registrador primário */
void asmPopOr()
{
    emit("POP BX");
    emit("OR AX, BX");
}

/* aplica OU-exclusivo com topo da pilha a registrador primário */
void asmPopXor()
{
    emit("POP BX");
    emit("XOR AX, BX");
}

/* aplica AND com topo da pilha e registrador primário */
void asmPopAnd()
{
    emit("POP BX");
    emit("AND AX, BX");
}

/* aplica NOT ao registrador primário */
void asmNot()
{
    emit("NOT AX");
}