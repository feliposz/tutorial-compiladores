#ifndef _CODEGEN_H
#define _CODEGEN_H

void AsmLoadConst(char *s);
void AsmLoadVar(char *s);
void AsmStoreVar(char *s);
void AsmNegate();
void AsmPush();
void AsmPopAdd();
void AsmPopSub();
void AsmPopMul();
void AsmPopDiv();
void AsmPopOr();
void AsmPopXor();
void AsmPopAnd();
void AsmNot();

#endif