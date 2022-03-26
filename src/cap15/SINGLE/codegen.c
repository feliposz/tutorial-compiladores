#include "output.h"
#include "codegen.h"

/* Carrega uma constante no registrador primário */
void AsmLoadConst(char c)
{
    EmitLn("MOV AX, %c", c);
}

/* Carrega uma variável no registrador primário */
void AsmLoadVar(char c)
{
    EmitLn("MOV AX, [%c]", c);
}