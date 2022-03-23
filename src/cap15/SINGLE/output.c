#include <stdio.h>
#include <stdarg.h>

/* emite uma instrução seguida por uma nova linha */
void EmitLn(char *fmt, ...)
{
    va_list args;
    
    putchar('\t');

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    putchar('\n');
}