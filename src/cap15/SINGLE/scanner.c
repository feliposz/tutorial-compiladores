#include <ctype.h>
#include "input.h"
#include "errors.h"
#include "scanner.h"

/* reconhece um operador aditivo */
int IsAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* reconhece um operador multiplicativo */
int IsMulOp(char c)
{
    return (c == '*' || c == '/');
}

/* verifica se caracter combina com o esperado */
void Match(char c)
{
    if (look != c)
        Expected("'%c'", c);
    NextChar();        
}

/* retorna um identificador */
char GetName()
{
    char name;
    
    if (!isalpha(look))
        Expected("Name");
    name = toupper(look);
    NextChar();
    
    return name;
}

/* retorna um número */
char GetNum()
{
    char num;
    
    if (!isdigit(look))
        Expected("Integer");
    num = look;
    NextChar();
    
    return num;
}