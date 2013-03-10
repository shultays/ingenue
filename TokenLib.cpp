

#include <stdlib.h>
#include <string.h>

#include "Tools.h"
#include "TokenLib.h"
#include "ValueLib.h"
#include "FlowLib.h"

char *tokenTypeString[] = {
  "err",
  "program",
  "integer",
  "float",
  "string",
  "operator",
  "variable",
  "value",
  "assignment",
  "statement",
  "ifcond",
  "else",
  "whileloop",
  "break",
};

tokenType singleTokens[] = {INTEGER, FLOAT, STRING, OPERATOR, VARIABLE};


int isSingle(tokenType t){
  for(int i=0; i<sizeof(singleTokens)/sizeof(singleTokens[0]); i++){
    if(t == singleTokens[i]) return 1; 
  }
  return 0;
}

Token* createToken(char *c){
  Token *t = (Token*)malloc(sizeof(Token));
  tokenType type = ERROR;
  for(int i=0; type==ERROR && i<sizeof(tokenTypeString)/sizeof(tokenTypeString[0]); i++) 
    if(strcmp(c, tokenTypeString[i]) == 0) type = (tokenType)i;
    
  t->type = type;
  t->nextSibling = NULL;
  t->firstChild = NULL;
  return t;
}


int buildProgram(Token **place){
  getToken();
  if(buff[0] == '/') return 0;
  *place = createToken(buff);
  int endloop = 0;
  if(isSingle((*place)->type)){
    getToken();
    switch((*place)->type){
      case INTEGER:
        (*place)->extra = malloc(sizeof(int));
        *((int*)(*place)->extra ) = (atoi(buff));
      break;
      case FLOAT:
        (*place)->extra = malloc(sizeof(float));
        *((float*)(*place)->extra ) = (atof(buff));
      break;
      case STRING:
        {
          int l=strlen(buff)-2;
          (*place)->extra = malloc(l+2);
          strncpy((char*) (*place)->extra, buff+1, l);
          ((char*) (*place)->extra)[l] = 0;
        }
      break;
      case OPERATOR:
        (*place)->extra = (void*)(getOpType(buff));
      break;
      case VARIABLE:
        (*place)->extra = (void*)(getVarIndex(buff));
      break;
        
    }; 
  }else{
    Token **tt = &((*place)->firstChild);
    while(1){
      if(buildProgram(tt) == 0) break;
      tt = &((*tt)->nextSibling);
    }
  }
  
  if((*place)->type == WHILE){
    configWhileToken(*place);  
  }else if((*place)->type == VALUE){
    configValueToken(*place);    
  }else if((*place)->type == IFCOND){
    configIfCondToken(*place);    
  }
  return 1;
}

void print(Token *t, int tab){
  if(!t) return;
  for(int i=0; i<tab; i++) printf("  ");
  printf("%s ", tokenTypeString[t->type]);
  switch(t->type){
    case INTEGER:
      printf("%d", *(int*)t->extra);
    break;
    case FLOAT:
      printf("%f", *(float*)t->extra);
    break;
    case STRING:
      printf("%s", (char*)t->extra);
    break;
    case VARIABLE:
      printf("%s", getVarName((int)t->extra));
    break;
    case OPERATOR:
      printf("%s", getOpTypeString((int)t->extra));
    break;
    case VALUE:
       /*printf("%d", *((int*)(v->extra)));*/
    break;
  }
  printf("\n");
  if(t->type == IFCOND){
    IfExtra *extra = (IfExtra*)t->extra;
    print(extra->cond, tab+1);
    print(extra->ifPart, tab+1);
    print(extra->elsePart, tab+1);
    print(t->nextSibling, tab);
  }else{
    print(t->firstChild, tab+1);
    print(t->nextSibling, tab);
  }

}
