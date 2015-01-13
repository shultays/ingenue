#ifndef VALUELIB_H
#define VALUELIB_H

#include "TokenLib.h"

typedef struct{
	int type;
	void *extra;
} Value;


int getVarIndex(char *c);
char* getVarName(int i);

int getOpType(char *c);

char* getOpTypeString(int t);

void configValueToken(Token *t);
void buildPostPreOpToken(Token *t);

Value* interpereteValue(Token *t);

Value* lastValue();


void getVar(char *a);
void resetCalc();
#endif
