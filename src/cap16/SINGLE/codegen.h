#ifndef _CODEGEN_H
#define _CODEGEN_H

void AsmLoadConst(char c);
void AsmLoadVar(char c);
void AsmStoreVar(char c);
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