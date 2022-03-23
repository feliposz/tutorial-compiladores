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