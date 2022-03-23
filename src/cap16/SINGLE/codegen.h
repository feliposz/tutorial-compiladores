#ifndef _CODEGEN_H
#define _CODEGEN_H

void asmLoadConstant(char c);
void asmLoadVariable(char c);
void asmStoreVariable(char c);
void asmNegate();
void asmPush();
void asmPopAdd();
void asmPopSub();
void asmPopMul();
void asmPopDiv();
void asmPopOr();
void asmPopXor();
void asmPopAnd();
void asmNot();

#endif