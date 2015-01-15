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



void configIfCondToken(Token *t);
void configWhileToken(Token *t);
void configDoWhileToken(Token *t);
void configForToken(Token *t);


int interpreteFlow(Token *t); 
#endif
