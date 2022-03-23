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