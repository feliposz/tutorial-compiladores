#include <stdio.h>
#include "input.h"

char Look; /* caracter "lookahead */

/* lê próximo caracter da entrada */
void NextChar()
{
    Look = getchar();
}

/* inicializa o módulo de entrada */
void InitInput()
{
    NextChar();
}
