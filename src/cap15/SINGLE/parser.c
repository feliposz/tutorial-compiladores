#include <ctype.h>
#include "input.h"
#include "scanner.h"
#include "codegen.h"
#include "errors.h"
#include "parser.h"

/* Analisa e traduz um fator matemático */
void Factor()
{
    if (isdigit(Look))
        AsmLoadConst(GetNum());
    else if (isalpha(Look))
        AsmLoadVar(GetName());
    else
        Error("Unrecognized character: '%c'", Look);
}