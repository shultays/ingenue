#ifndef VALUELIB_H
#define VALUELIB_H

#include "TokenLib.h"

typedef struct{
	int type;
	void *extra;
} Value;


int getVarIndex(char *c);
char* getVarName(int i);

int getOpType(char *c, bool isSingle = false);

char* getOpTypeString(int t);

void configValueToken(Token *t);
void buildPostPreOpToken(Token **place);

Value* interpereteValue(Token *t);

Value* lastValue();


void getVar(char *a);
void resetCalc();
#endif
