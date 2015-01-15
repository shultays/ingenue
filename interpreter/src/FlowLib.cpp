#include "FlowLib.h"
#include "TokenLib.h"
#include "ValueLib.h"

#include <stdlib.h>
#include <stdio.h>

int valueToBool(Value* v)
{
	switch (v->type)
	{
		case INTEGER:
			return *((int*)v->extra) != 0;
		case FLOAT:
			return *((float*)v->extra) != 0.0f;
		case STRING:
			return ((char*)v->extra)[0] != '\0';
	}
	return 0;
}

int interpreteFlow(Token *t){
	Value *v, *v2, *v3;
	if(t==NULL) return 0;
	int result = 0;
	switch(t->type){
		default :
			result=interpreteFlow(t->firstChild);
		break;
		case STATEMENT:
		{
			result = interpreteFlow(t->firstChild);
		}
		break;
		case BREAK:
		{
			return 1;
		}
			break;
		case WHILE:
		{
			WhileExtra *extra = (WhileExtra*)t->extra;
			while (valueToBool(interpereteValue(extra->cond))){
				interpreteFlow(extra->statements);
			}
		}
		break;
		case DOWHILE:
		{
			DoWhileExtra *extra = (DoWhileExtra*)t->extra;
			do {
				interpreteFlow(extra->statements);
			} while(valueToBool(interpereteValue(extra->cond)));
		}
		break;
		case IFCOND:
		{
			IfExtra *extra = (IfExtra*)t->extra;
			Value *v = interpereteValue(extra->cond);
			int condResult = 0;
			switch(v->type){ //TODO this should check other states
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
		case FOR:
		{
			ForExtra *extra = (ForExtra*)t->extra;
			for (interpereteValue(extra->initPart); valueToBool(interpereteValue(extra->condPart)); interpereteValue(extra->afterPart)){
				if(interpreteFlow(extra->statements->firstChild)) break;
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



void configForToken(Token *t){
	Token *temp = t->firstChild;
	t->extra = malloc(sizeof(ForExtra));
	((ForExtra*)t->extra)->statements = temp->nextSibling->nextSibling->nextSibling;
	((ForExtra*)t->extra)->afterPart = temp->nextSibling->nextSibling;
	((ForExtra*)t->extra)->afterPart->nextSibling = NULL;
	((ForExtra*)t->extra)->condPart = temp->nextSibling;
	((ForExtra*)t->extra)->condPart->nextSibling = NULL;
	((ForExtra*)t->extra)->initPart = temp;
	((ForExtra*)t->extra)->initPart->nextSibling = NULL;
}


void configWhileToken(Token *t){
	Token *temp = t->firstChild;
	t->extra = malloc(sizeof(WhileExtra));
	((WhileExtra*)t->extra)->cond = temp;
	((WhileExtra*)t->extra)->statements = temp->nextSibling;
	temp->nextSibling=NULL;
}

void configDoWhileToken(Token *t){
	Token *temp = t->firstChild;
	t->extra = malloc(sizeof(DoWhileExtra));
	((DoWhileExtra*)t->extra)->cond = temp->nextSibling;
	((DoWhileExtra*)t->extra)->statements = temp;
	temp->nextSibling = NULL;
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
