#include <ctype.h>
#include "input.h"
#include "errors.h"
#include "scanner.h"

/* reconhece um operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-' || c == '|' || c == '~');
}

/* reconhece um operador multiplicativo */
int isMulOp(char c)
{
    return (c == '*' || c == '/' || c == '&');
}

/* verifica se caracter combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();        
}

/* retorna um identificador */
char getName()
{
    char name;
    
    if (!isalpha(look))
        expected("Name");
    name = toupper(look);
    nextChar();
    
    return name;
}

/* retorna um número */
char getNum()
{
    char num;
    
    if (!isdigit(look))
        expected("Integer");
    num = look;
    nextChar();
    
    return num;
}