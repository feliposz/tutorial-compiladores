#include "output.h"
#include "codegen.h"

/* carrega uma constante no registrador primário */
void AsmLoadConst(char *s)
{
    EmitLn("MOV AX, %s", s);
}

/* carrega uma variável no registrador primário */
void AsmLoadVar(char *s)
{
    EmitLn("MOV AX, %s", s);
}