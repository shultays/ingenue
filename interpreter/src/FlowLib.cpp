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
			result=interpreteFlow(t->firstChild);
		}
		break;
		case RETURN:
		{
			interpreteFlow(t->firstChild);
			return 1;
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
		case FUNC_DEF:
		case FUNC_CALL:
			interpereteValue(t);
		break;

		case VALUE:
		case ASSIGNMENT:
			interpereteValue(t->firstChild);
		break;
	}
	if(result) return result;
	return interpreteFlow(t->nextSibling);

}



void configForToken(Token *t){
	Token *temp = t->firstChild;
	ForExtra *extra = (ForExtra*)(t->extra = malloc(sizeof(ForExtra)));

	extra->statements = temp->nextSibling->nextSibling->nextSibling;
	extra->afterPart = temp->nextSibling->nextSibling;
	extra->afterPart->nextSibling = NULL;
	extra->condPart = temp->nextSibling;
	extra->condPart->nextSibling = NULL;
	extra->initPart = temp;
	extra->initPart->nextSibling = NULL;
}


void configWhileToken(Token *t){
	Token *temp = t->firstChild;
	WhileExtra *extra = (WhileExtra*)(t->extra = malloc(sizeof(WhileExtra)));
	extra->cond = temp;
	extra->statements = temp->nextSibling;
	temp->nextSibling=NULL;
}

void configDoWhileToken(Token *t){
	Token *temp = t->firstChild;
	DoWhileExtra *extra = (DoWhileExtra*)(t->extra = malloc(sizeof(DoWhileExtra)));
	extra->cond = temp->nextSibling;
	extra->statements = temp;
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


void configFuncDefToken(Token *t){
	Token *temp = t->firstChild->nextSibling;
	FuncDefExtra *extra = (FuncDefExtra*)(t->extra = malloc(sizeof(FuncDefExtra)));
	extra->name = t->firstChild;
	extra->name->nextSibling = NULL;
	Token **place = &(extra->parameters);
	while (temp->type != STATEMENT)
	{
		*place = temp;
		place = &((*place)->nextSibling);
		temp = temp->nextSibling;
	}
	*place = NULL;
	extra->func_body = temp;
	t->firstChild = NULL;
}


void configFuncCallToken(Token *t){
	Token *temp = t->firstChild->nextSibling;
	FuncCallExtra *extra = (FuncCallExtra*)(t->extra = malloc(sizeof(FuncCallExtra)));
	extra->name = t->firstChild;
	extra->name->nextSibling = NULL;
	extra->values = temp;
	t->firstChild = NULL;
}





