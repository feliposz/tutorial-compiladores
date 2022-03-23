#include "output.h"
#include "codegen.h"

/* carrega uma constante no registrador primário */
void asmLoadConstant(char c)
{
    emit("MOV AX, %c", c);
}

/* carrega uma variável no registrador primário */
void asmLoadVariable(char c)
{
    emit("MOV AX, %c", c);
}