#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "errors.h"

/* exibe uma mensagem de erro formatada */
void error(char *fmt, ...)
{
    va_list args;
    
    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    fputc('\n', stderr);

    exit(1);
}

/* alerta sobre alguma entrada esperada */
void expected(char *fmt, ...)
{
    va_list args;
    
    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    fputs(" expected!\n", stderr);
    
    exit(1);
}