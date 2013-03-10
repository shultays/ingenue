#include "FlowLib.h"
#include "TokenLib.h"
#include "ValueLib.h"

#include <stdlib.h>
#include <stdio.h>


int interpreteFlow(Token *t){
  Value *v, *v2, *v3;
  if(t==NULL) return 0;
  int result = 0;
  switch(t->type){    
    default :
      result=interpreteFlow(t->firstChild);
    break;
    case BREAK:
    {
      return 1;
    }
    break;
    case WHILE:
    {
      WhileExtra *extra = (WhileExtra*)t->extra;
      while(true){
        Value *v = interpereteValue(extra->cond);
        int condResult = 0;
        switch(v->type){
          case INTEGER:
            condResult = *((int*)v->extra);
          break;
        }
        if(condResult==0)break;
        if(interpreteFlow(extra->statements))break;
      }
    }
    break;
    case IFCOND:
    {
      IfExtra *extra = (IfExtra*)t->extra;
      Value *v = interpereteValue(extra->cond);
      int condResult = 0;
      switch(v->type){
        case INTEGER:
          condResult = *((int*)v->extra);
        break;
      }
      if(condResult){
          result=interpreteFlow(extra->ifPart);
      }else{
          result=interpreteFlow(extra->elsePart);
      }
    }
    break;
    case VALUE:
    case ASSIGNMENT:
      resetCalc();
      interpereteValue(t->firstChild);
    break;
  }
  if(result) return result;
  return interpreteFlow(t->nextSibling);

}

void configWhileToken(Token *t){
  Token *temp = t->firstChild;
  t->extra = malloc(sizeof(WhileExtra));
  ((WhileExtra*)t->extra)->cond = temp;
  ((WhileExtra*)t->extra)->statements = temp->nextSibling;
  temp->nextSibling=NULL;
}

void configIfCondToken(Token *t){
  Token *temp = t->firstChild;
  t->extra = malloc(sizeof(IfExtra));
  ((IfExtra*)t->extra)->cond = temp;

  ((IfExtra*)t->extra)->ifPart = temp->nextSibling;
  Token **tt = &(((IfExtra*)t->extra)->ifPart);
  ((IfExtra*)t->extra)->elsePart = NULL;
  while(*tt){
    if((*tt)->type == ELSE){
      ((IfExtra*)t->extra)->elsePart = (*tt)->nextSibling;
      *tt=NULL;
      break;
    }
    tt = &(*tt)->nextSibling;
  }
  temp->nextSibling = NULL;
}
