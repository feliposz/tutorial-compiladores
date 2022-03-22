#include <ctype.h>
#include "input.h"
#include "scanner.h"
#include "codegen.h"
#include "errors.h"
#include "parser.h"

/* analisa e traduz um fator matemático */
void factor()
{
    char name[MAXNAME+1], num[MAXNUM+1];

    if (isdigit(look)) {
        getNum(num);
        asmLoadConstant(num);
    } else if (isalpha(look)) {
        getName(name);
        asmLoadVariable(name);
    } else
        error("Unrecognized character: '%c'", look);
}