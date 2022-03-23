#include <ctype.h>
#include "input.h"
#include "scanner.h"
#include "codegen.h"
#include "errors.h"
#include "parser.h"

/* analisa e traduz um fator matemático */
void Factor()
{
    char name[MAXNAME+1], num[MAXNUM+1];

    if (isdigit(look)) {
        GetNum(num);
        AsmLoadConst(num);
    } else if (isalpha(look)) {
        GetName(name);
        AsmLoadVar(name);
    } else
        Error("Unrecognized character: '%c'", look);
}