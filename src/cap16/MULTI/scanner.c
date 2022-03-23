#include <ctype.h>
#include "input.h"
#include "errors.h"
#include "scanner.h"

/* reconhece um operador aditivo */
int IsAddOp(char c)
{
    return (c == '+' || c == '-' || c == '|' || c == '~');
}

/* reconhece um operador multiplicativo */
int IsMulOp(char c)
{
    return (c == '*' || c == '/' || c == '&');
}

/* verifica se caracter combina com o esperado */
void Match(char c)
{
    if (Look != c)
        Expected("'%c'", c);
    NextChar();        
}

/* retorna um identificador */
void GetName(char *name)
{
    int i;
    
    if (!isalpha(Look))
        Expected("Name");
    for (i = 0; isalnum(Look); i++) {
        if (i >= MAXNAME)
            Error("Identifier too long.");
        name[i] = toupper(Look);
        NextChar();
    }
    name[i] = '\0';
}

/* retorna um número */
void GetNum(char *num)
{
    int i;
    
    if (!isdigit(Look))
        Expected("Integer");
    for (i = 0; isdigit(Look); i++) {
        if (i >= MAXNUM)
            Error("Integer too large.");
        num[i] = Look;
        NextChar();
    }
    num[i] = '\0';
}