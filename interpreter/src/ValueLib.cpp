#include "ValueLib.h"
#include "FlowLib.h"
#include "Tools.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <string>
#include <stack>
#include <unordered_map>
#include <assert.h> 

int currentStack=0;
std::unordered_map<std::string, Value*> variables[512];

int variablesStackBuffSize[512] = { 0 };
char variableValueBuff[512*1024];


char valueExtraBuff[1024*1024*5];
int nValueExtraBuff=0;


Value valueBuff[256];
int nValueBuff;


std::stack<Value*> valueStack;


Value* getVal(std::string &name)
{
	int st = currentStack;
	while (st >= 0)
	{
		std::unordered_map<std::string, Value*>::iterator it;
		it = variables[st].find(name);
		if (it != variables[st].end())
		{
			return it->second;
		}
		st--;
	}
	Value *ret = (Value*)malloc(sizeof(Value));
	ret->type = ERROR;
	variables[currentStack].emplace(name, ret);

	return ret;
}

typedef enum{
	ERROROP,
	ADDITION, 
	SUBTRACTION,
	MULTIPLICATION,
	DIVISION,
	MODULO,
	EQUALITY,
	INEQUALITY,
	GREATER_THAN,
	GREATER_OR_EQUAL,
	LESS_THAN,
	LESS_OR_EQUAL,

	POST_INC,
	PRE_INC,
	POST_DEC,
	PRE_DEC,

	INC,
	DEC,

	NEGATE,
	PLUS,
	NOT,
	
	MAXOP
} opType;

int operatorPrecedences[] = {
	-1,
	1, //+
	1, //-
	2, //mul
	2, //dif
	2, //mod


	3, //==
	3, //!=
	3, //>
	3, //>=
	3, //<
	3, //<=

	10, //i++
	10, //++i
	10, //i--
	10, //--i

	10, //special
	10, //special

	11, //negate
	11, //plus
	11, //not

	-1
};


char *opTypeString[] = {
	"err",
	"+",
	"-",
	"*",
	"/",
	"%",
	"==",
	"!=",
	">",
	">=",
	"<",
	"<=",

	"post++", //rest are unary
	"pre++",
	"post--",
	"pre--",
	"++", //these are only used for covnerting
	"--",

	"-", //unary
	"+", //unary
	"!", //unary
};

int isOperatorSingle(int op)
{
	return op >= POST_INC && op < MAXOP;
}
int compare(int op, int a, int b)
{
	switch (op){
		case EQUALITY:
			return a == b;
		case INEQUALITY:
			return a != b;
		case GREATER_THAN:
			return a > b;
		case GREATER_OR_EQUAL:
			return a >= b;
		case LESS_THAN:
			return a < b;
		case LESS_OR_EQUAL:
			return a <= b;
	}
	PERROR("Wrong comparison OP");
	return 0;
}
int comparef(int op, float a, float b)
{
	switch (op){
		case EQUALITY:
			return a == b;
		case INEQUALITY:
			return a != b;
		case GREATER_THAN:
			return a > b;
		case GREATER_OR_EQUAL:
			return a >= b;
		case LESS_THAN:
			return a < b;
		case LESS_OR_EQUAL:
			return a <= b;
	}
	PERROR("Wrong float comparison OP");
	return 0;
}

void convert(Value *v, int newType){
	switch(v->type){
		case INTEGER:
		{
			int a = *((int*)v->extra);
			switch(newType){
				case FLOAT:
					v->extra = valueExtraBuff+nValueExtraBuff;
					nValueExtraBuff+=sizeof(float);
					*((float*)v->extra) = (float)a;
				break;
				case STRING:
					v->extra = valueExtraBuff+nValueExtraBuff;
					nValueExtraBuff+=sprintf((char*)v->extra, "%d", a)+1;
					break;
				default:
					PERROR("");
				break;
			}
		}
		break;
		case FLOAT:
		{
			float a = *((float*)v->extra);
			switch(newType){
				case INTEGER:
					v->extra = valueExtraBuff+nValueExtraBuff;
					nValueExtraBuff+=sizeof(int);
					*((int*)v->extra) = (int)a;
				break;
				case STRING:
					v->extra = valueExtraBuff+nValueExtraBuff;
					nValueExtraBuff+=sprintf((char*)v->extra, "%f", a)+1;
					break;
				default:
					PERROR("");
				break;
			}
		}
		break;
		case STRING:
		{
			char *a = (char*)v->extra;
			switch(newType){
				case FLOAT:
					v->extra = valueExtraBuff+nValueExtraBuff;
					nValueExtraBuff+=sizeof(float);
					*((float*)v->extra) = (float)atof(a);
				break;
				case INTEGER:
					v->extra = valueExtraBuff+nValueExtraBuff;
					nValueExtraBuff+=sizeof(int);
					*((int*)v->extra) = atoi(a);
				break;
				default:
					PERROR("");
				break;
			}
		}
		break;
		default:
			PERROR("");
		break;

	}
	v->type = newType;
}

Value* interpreteAssignment(std::string name, int type, void *extra){
	Value *v2;
	v2 = getVal(name);
	
	if (type == STRING){
		v2->type = STRING;
		v2->extra = (void*)(variableValueBuff + variablesStackBuffSize[currentStack]);
		variablesStackBuffSize[currentStack] += strlen((char*)extra) + 1;
		strcpy((char*)v2->extra, (char*)extra);
	}
	else{
		if (v2->type != type){
			v2->type = type;
			switch (v2->type){
				case INTEGER:
					v2->extra = (void*)(variableValueBuff + variablesStackBuffSize[currentStack]);
					variablesStackBuffSize[currentStack] += sizeof(int);
					break;
				case FLOAT:
					v2->extra = (void*)(variableValueBuff + variablesStackBuffSize[currentStack]);
					variablesStackBuffSize[currentStack] += sizeof(float);
					break;
			}
		}
		switch (v2->type){
			case INTEGER:
				*((int*)v2->extra) = *((int*)extra);
				break;
			case FLOAT:
				*((float*)v2->extra) = *((float*)extra);
				break;
			default:
				PERROR("");
				break;
		}
	}
	return v2;
}

Value* interpereteStringOperator(Token *t, Value *v2, Value *v3){

	switch ((int)t->extra){
		case ADDITION:
		{
			char *c2 = (char*)v2->extra, *c3 = (char*)v3->extra;
			v2->extra = valueExtraBuff + nValueExtraBuff;
			nValueExtraBuff += sprintf((char*)v2->extra, "%s%s", c2, c3) + 1;
		}
		break;
		
		case EQUALITY:
		case INEQUALITY:
		case GREATER_THAN:
		case GREATER_OR_EQUAL:
		case LESS_THAN:
		case LESS_OR_EQUAL:
		{
			char *c2 = (char*)v2->extra, *c3 = (char*)v3->extra;
			v2->type = INTEGER;
			*((int*)v2->extra) = compare((int)t->extra, strcmp(c2, c3), 0);
		}
		break;
		default:
			PERROR("");
		break;
	}
	return v2;
}

Value* interpereteFloatOperator(Token *t, Value *v2, Value *v3){
	switch ((int)t->extra){
		case ADDITION:
			*((float*)v2->extra) += *((float*)v3->extra);
			break;
		case SUBTRACTION:
			*((float*)v2->extra) -= *((float*)v3->extra);
			break;
		case MULTIPLICATION:
			*((float*)v2->extra) *= *((float*)v3->extra);
			break;
		case DIVISION:
			*((float*)v2->extra) /= *((float*)v3->extra);
			break;
		case EQUALITY:
		case INEQUALITY:
		case GREATER_THAN:
		case GREATER_OR_EQUAL:
		case LESS_THAN:
		case LESS_OR_EQUAL:
			v2->type = INTEGER;
			*((int*)v2->extra) = comparef((int)t->extra, *((float*)v2->extra), *((float*)v3->extra));
			break;
		case PRE_INC:
		case PRE_DEC:
		case POST_INC:
		case POST_DEC:
		{
			v2 = v3;
			float old_val = *((float*)v2->extra);
			float new_val;
			if ((int)t->extra == POST_INC || (int)t->extra == PRE_INC){
				new_val = old_val + 1.0f;
			}
			else{
				new_val = old_val - 1.0f;
			}
			interpreteAssignment(getVarName((int)t->firstChild->extra), FLOAT, &new_val);

			if ((int)t->extra == PRE_INC || (int)t->extra == PRE_DEC){
				*((float*)v2->extra) = new_val;
			}
		}
		break;
		case NEGATE:
			v2 = v3;
			*((float*)v2->extra) = -*((float*)v2->extra);
			break;
		case PLUS:
			v2 = v3;
			break;
		default:
			PERROR("");
		break;
	}
	return v2;
}

Value* interpereteIntegerOperator(Token *t, Value *v2, Value *v3){
	switch ((int)t->extra){
		case ADDITION:
			*((int*)v2->extra) += *((int*)v3->extra);
		break;
		case SUBTRACTION:
			*((int*)v2->extra) -= *((int*)v3->extra);
		break;
		case MULTIPLICATION:
			*((int*)v2->extra) *= *((int*)v3->extra);
		break;
		case DIVISION:
			*((int*)v2->extra) /= *((int*)v3->extra);
		break;
		case MODULO:
			*((int*)v2->extra) %= *((int*)v3->extra);
		break;
		case EQUALITY:
		case INEQUALITY:
		case GREATER_THAN:
		case GREATER_OR_EQUAL:
		case LESS_THAN:
		case LESS_OR_EQUAL:
			*((int*)v2->extra) = comparef((int)t->extra, *((float*)v2->extra), *((float*)v3->extra));
			break;
		case PRE_INC:
		case PRE_DEC:
		case POST_INC:
		case POST_DEC:
		{
			v2 = v3;
			int old_val = *((int*)v2->extra);
			int new_val;
			if ((int)t->extra == POST_INC || (int)t->extra == PRE_INC){
				new_val = old_val + 1;
			 }else{
				new_val = old_val - 1;
			}
			interpreteAssignment(getVarName((int)t->firstChild->extra), INTEGER, &new_val);

			if ((int)t->extra == PRE_INC || (int)t->extra == PRE_DEC){
				*((int*)v2->extra) = new_val;
			}
		}
			break;
		case NEGATE:
			v2 = v3;
			*((int*)v2->extra) = -*((int*)v2->extra);
			break;
		case PLUS:
			v2 = v3;
			break;
		case NOT:
			v2 = v3;
			*((int*)v2->extra) = !*((int*)v2->extra);
			break;
		default:
			PERROR("");
		break;
	}
	return v2;
}


void printValues(Token *token)
{
	while (token)
	{
		interpereteValue(token->firstChild);
		Value *val = valueBuff + nValueBuff;
		if (val->type != STRING)
		{
			convert(val, STRING);
		}
		printf("%s", (char*)val->extra);
		token = token->nextSibling;
	}
}

Value* interpereteValue(Token *t){
	Value *v = NULL, *v2 = NULL, *v3 = NULL;
	if(t==NULL) return NULL;
	int nValueBuffOld=nValueBuff;
	int nValueExtraBuffOld = nValueExtraBuff;

	switch (t->type){
		default:
			PERRORTOK("", t);
			v=interpereteValue(t->firstChild);
			break;
		case VALUE:
			v=interpereteValue(t->firstChild);
			break;
		case ASSIGNMENT:
		{
			interpereteValue(t->firstChild->nextSibling);
			v2 = valueBuff + nValueBuff;
			v = interpreteAssignment(getVarName((int)t->firstChild->extra), v2->type, v2->extra);
			break;
		}
		case VARIABLE:
		{
			std::string name(getVarName((int)t->extra));
			v2 = getVal(name);
			v = valueBuff+nValueBuff++;
			switch(v2->type){
				case INTEGER:
					v->type = INTEGER;
					v->extra = valueExtraBuff+nValueExtraBuff;
					*((int*)v->extra) = *((int*)v2->extra);
					nValueExtraBuff+=sizeof(int);
				break;
				case FLOAT:
					v->type = FLOAT;
					v->extra = valueExtraBuff+nValueExtraBuff;
					*((float*)v->extra) = *((float*)v2->extra);
					nValueExtraBuff+=sizeof(float);
				break;
				case STRING:
					v->type = STRING;
					v->extra = valueExtraBuff+nValueExtraBuff;
					strcpy((char*)v->extra, (char*)v2->extra);
					nValueExtraBuff+=strlen((char*)v->extra)+1;
				break;
			}
		}
		break;
		case STRING:
			v = valueBuff+nValueBuff++;
			v->type = STRING;
			v->extra = valueExtraBuff+nValueExtraBuff;
			strcpy((char*)v->extra, (char*)t->extra);
			nValueExtraBuff+=strlen((char*)v->extra)+1;
		break;
		case INTEGER:
			v = valueBuff+nValueBuff++;
			v->type = INTEGER;
			v->extra = valueExtraBuff+nValueExtraBuff;
			*((int*)v->extra) = *((int*)t->extra);
			nValueExtraBuff+=sizeof(int);
		break;
		case FLOAT:
			v = valueBuff+nValueBuff++;
			v->type = FLOAT;
			v->extra = valueExtraBuff+nValueExtraBuff;
			*((float*)v->extra) = *((float*)t->extra);
			nValueExtraBuff+=sizeof(float);
		break;
		case OPERATOR:
			v3 = valueBuff + nValueBuff - 1;
			if (!isOperatorSingle((int)t->extra))
			{
				v2 = valueBuff+nValueBuff-2;
				int type = (int)t->extra;
				if(v2->type < v3->type){
					convert(v2, (int)v3->type);
				}else if(v2->type > v3->type){
					convert(v3, (int)v2->type);
				}
			}
			switch (v3->type){
				case STRING:
					v = interpereteStringOperator(t, v2, v3);
					break;
				case FLOAT:
					v = interpereteFloatOperator(t, v2, v3);
					break;
				case INTEGER:
					v = interpereteIntegerOperator(t, v2, v3);
					break;
			}

			if (!isOperatorSingle((int)t->extra))
				nValueBuff--;
			
		break;
		case FUNC_DEF:
		{
			FuncDefExtra *extra = (FuncDefExtra*)t->extra;
			std::string name(getVarName((int)extra->name->extra));
			v2 = getVal(name);
			v2->type = FUNC_DEF;
			v2->extra = t;
		}
		break;
		case FUNC_CALL:
		{
			FuncCallExtra *extra = (FuncCallExtra*)t->extra;
			std::string name(getVarName((int)extra->name->extra));
			if (name == "print")
			{
				printValues(extra->values);
			}
			else
			{
				Value *val = getVal(name);
				currentStack++;
				Token *funcDefToken = (Token*)val->extra;
				FuncDefExtra *defExtra = (FuncDefExtra*)funcDefToken->extra;

				Token *t_val = extra->values;
				Token *t_def = defExtra->parameters;
				while (t_val)
				{
					interpereteValue(t_val->firstChild);
					v2 = valueBuff + nValueBuff;

					interpreteAssignment(getVarName((int)t_def->extra), v2->type, v2->extra);
					t_val = t_val->nextSibling;
					t_def = t_def->nextSibling;
				}

				interpreteFlow(defExtra->func_body);
				currentStack--;
			}
		}
		break;
	}
	v2 = interpereteValue(t->nextSibling);
	nValueBuff = nValueBuffOld;
	nValueExtraBuff = nValueExtraBuffOld;
	return v2?v2:v;
}

char variableNames[128][32];
int currentVar=0;


int getVarIndex(char *c){
	for(int i=0; i<currentVar; i++) if(strcmp(variableNames[i], c)==0) return i;
	strcpy(variableNames[currentVar], c);
	currentVar++;
	return currentVar-1;
}

char *getVarName(int i){
	return variableNames[i];	
}


int getOpType(char *c, bool is_single){
	int start = 0;
	if (is_single){
		start = NEGATE;
	}
	opType op = ERROROP;	
	for(int i=start; op==ERROROP && i<MAXOP; i++) 
		if(strcmp(c, opTypeString[i]) == 0) op = (opType) i; 

	return op;
}

char *getOpTypeString(int t){
	return opTypeString[t];
}

Value* lastValue(){
	if(nValueBuff==0) return NULL;
	else return valueBuff + nValueBuff-1;
}

void configValueToken(Token *t){
	std::queue <Token*> newOrder;
	std::stack <Token*> opStack;
	Token *it = t->firstChild;

	Token **temp;

	while(it != NULL){
		if(it->type != OPERATOR){
			newOrder.push(it);
		}else{
			while (!opStack.empty() && operatorPrecedences[(int)opStack.top()->extra] > operatorPrecedences[(int)it->extra])
			{
				newOrder.push(opStack.top());
				opStack.pop();
			}
			opStack.push(it);
		}
		it = it->nextSibling;
	}
	temp = &t->firstChild;
	while (!newOrder.empty())
	{
		*temp = newOrder.front();
		temp = &((*temp)->nextSibling);
		newOrder.pop();
	}
	while (!opStack.empty())
	{
		*temp = opStack.top();
		temp = &((*temp)->nextSibling);
		opStack.pop();
	}
	*temp=NULL;
}

void buildPostPreOpToken(Token **place){

	Token *t = *place;

	if (t->type == VAR_WITH_PRE_OP)
	{
		if ((int)t->firstChild->extra == INC)
		{
			t->extra = (void*)PRE_INC;
		}
		else
		{
			t->extra = (void*)PRE_DEC;
		}
		t->firstChild = t->firstChild->nextSibling;
		t->firstChild->nextSibling = NULL;
	}
	else
	{
		if ((int)t->firstChild->nextSibling->extra == INC)
		{
			t->extra = (void*)POST_INC;
		}
		else
		{
			t->extra = (void*)POST_DEC;
		}
		t->firstChild->nextSibling = NULL;
	}
	t->type = OPERATOR;
	*place = t->firstChild;
	(*place)->nextSibling = t;
}



void getVar(char *a){
	std::string name(a);
	Value *v = variables[currentStack][name];
	if(v==NULL) return;
	switch(v->type){
		case INTEGER:
			printf("%s %d\n",a, *((int*)variables[currentStack][name]->extra));
		break;
		case FLOAT:
			printf("%s %f\n",a, *((float*)variables[currentStack][name]->extra));
		break;
		case STRING:
			printf("%s %s\n",a, ((char*)variables[currentStack][name]->extra));
		break;
	}
}
