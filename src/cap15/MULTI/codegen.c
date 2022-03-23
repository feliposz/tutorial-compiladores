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