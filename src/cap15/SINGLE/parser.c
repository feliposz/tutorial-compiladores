#include <ctype.h>
#include "input.h"
#include "scanner.h"
#include "codegen.h"
#include "errors.h"
#include "parser.h"

/* analisa e traduz um fator matemático */
void factor()
{
    if (isdigit(look))
        asmLoadConstant(getNum());
    else if (isalpha(look))
        asmLoadVariable(getName());
    else
        error("Unrecognized character: '%c'", look);
}