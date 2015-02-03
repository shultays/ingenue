

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
	"dowhileloop",
	"break",
	"return",
	"forloop",
	"variable_withpost",
	"variable_withpre",
	"singleoperator",
	"unary",
	"funcdef",
	"funccall"
};

tokenType singleTokens[] = {INTEGER, FLOAT, STRING, OPERATOR, VARIABLE, SINGLE_OPERATOR};


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
				*((float*)(*place)->extra ) = (float)(atof(buff));
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
			case SINGLE_OPERATOR:
			{
				(*place)->extra = (void*)(getOpType(buff, true));
				(*place)->type = OPERATOR;
			}
			break;
			case UNARY:
			{

			}
			break;
		}; 
	}else{
		Token **tt = &((*place)->firstChild);
		while(1){
			if(buildProgram(tt) == 0) break;
			while(*tt) tt = &((*tt)->nextSibling);
		}
	}
	
	if ((*place)->type == WHILE){
		configWhileToken(*place);
	}else if ((*place)->type == DOWHILE){
		configDoWhileToken(*place);
	}else if((*place)->type == VALUE){
		configValueToken(*place);		
	}else if((*place)->type == IFCOND){
		configIfCondToken(*place);		
	}else if ((*place)->type == FOR){
		configForToken(*place);
	}else if ((*place)->type == VAR_WITH_POST_OP || (*place)->type == VAR_WITH_PRE_OP){
		buildPostPreOpToken(place);
	}else if ((*place)->type == FUNC_DEF){
		configFuncDefToken(*place);
	}else if ((*place)->type == FUNC_CALL){
		configFuncCallToken(*place);
	}
	return 1;
}

int l;

void print(Token *t, int tab, int line){
	if(!t) return;
	t->line = l;
	printf("%2d ", l++);
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
		print(extra->cond, tab + 1, ++line);
		print(extra->ifPart, tab + 1, ++line);
		print(extra->elsePart, tab + 1, ++line);
		print(t->nextSibling, tab, ++line);
	}else{
		if (t->type != OPERATOR)
			print(t->firstChild, tab + 1, ++line);
		print(t->nextSibling, tab, ++line);
	}

}
