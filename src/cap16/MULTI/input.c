#include <stdio.h>
#include "input.h"

char Look; /* Caracter "lookahead */

/* Lê próximo caracter da entrada */
void NextChar()
{
    Look = getchar();
}

/* Inicializa o módulo de entrada */
void InitInput()
{
    NextChar();
}
