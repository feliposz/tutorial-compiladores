#include <stdio.h>
#include "input.h"

char look; /* caracter "lookahead */

/* lê próximo caracter da entrada */
void NextChar()
{
    look = getchar();
}

/* inicializa o módulo de entrada */
void InitInput()
{
    NextChar();
}
