#ifndef _SCANNER1_H
#define _SCANNER1_H

#define MAXNAME 30
#define MAXNUM 5

int IsAddOp(char c);
int IsMulOp(char c);

void Match(char c);
void GetName(char *name);
void GetNum(char *num);

#endif