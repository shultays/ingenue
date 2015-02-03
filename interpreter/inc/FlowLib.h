#ifndef FLOWLIB_H
#define FLOWLIB_H

#include "FlowLib.h"
#include "TokenLib.h"

typedef struct{
	Token *cond;
	Token *ifPart;
	Token *elsePart;
}IfExtra;


typedef struct{
	Token *initPart;
	Token *condPart;
	Token *afterPart;
	Token *statements;
}ForExtra;

typedef struct{
	Token *cond;
	Token *statements;
}WhileExtra, DoWhileExtra;


typedef struct{
	Token *name;
	Token *parameters;
	Token *func_body;
}FuncDefExtra;


typedef struct{
	Token *name;
	Token *values;
}FuncCallExtra;


void configIfCondToken(Token *t);
void configWhileToken(Token *t);
void configDoWhileToken(Token *t);
void configForToken(Token *t);
void configFuncDefToken(Token *t);
void configFuncCallToken(Token *t);

int interpreteFlow(Token *t); 
#endif
