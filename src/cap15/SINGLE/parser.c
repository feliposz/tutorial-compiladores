#include <ctype.h>
#include "input.h"
#include "scanner.h"
#include "codegen.h"
#include "errors.h"
#include "parser.h"

/* analisa e traduz um fator matemático */
void Factor()
{
    if (isdigit(look))
        AsmLoadConst(GetNum());
    else if (isalpha(look))
        AsmLoadVar(GetName());
    else
        Error("Unrecognized character: '%c'", look);
}