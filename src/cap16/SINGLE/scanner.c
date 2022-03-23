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
char GetName()
{
    char name;
    
    if (!isalpha(Look))
        Expected("Name");
    name = toupper(Look);
    NextChar();
    
    return name;
}

/* retorna um número */
char GetNum()
{
    char num;
    
    if (!isdigit(Look))
        Expected("Integer");
    num = Look;
    NextChar();
    
    return num;
}