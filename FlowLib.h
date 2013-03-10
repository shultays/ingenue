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
  Token *cond;
  Token *statements;
}WhileExtra;

void configIfCondToken(Token *t); 
void configWhileToken(Token *t);   
 
int interpreteFlow(Token *t); 
#endif
