#ifndef PROGRAM_BUILDER_H
#define PROGRAM_BUILDER_H

#include <queue>
#include <stack>
#include "Tools.h"



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

	10, //++
	10, //--

	10,
	10,
	10,
	10,

	11,
	11,
	11,

	-1
};

class ProgramBuilder {
	char strnull[64];
	MemoryAllocator allocator;

	static OperatorType getOperatorType(Token& tokenData) {
		int op = 0;

		while (op < Op_count) {
			const char *str = getOperatorString(op);
			int len = strlen(str);
			if (len == tokenData.tokenLength) {

				if (memcmp(tokenData.tokenBegin, getOperatorString(op), len) == 0) {
					return (OperatorType)op;
				}

			}


			op++;
		}
		return Op_err;
	}

	void configureWhileObject(Object* object) {
		Object *temp = object->firstChild;
		object->whileExtra = allocator.getMemory<WhileExtra>();
		object->whileExtra->cond = temp;
		object->whileExtra->statements = temp->nextSibling;
		temp->nextSibling = nullptr;
		object->firstChild = nullptr;
	}

	void configureDoWhileObject(Object* object) {
		Object *temp = object->firstChild;
		object->doWhileExtra = allocator.getMemory<DoWhileExtra>();
		object->doWhileExtra->cond = temp->nextSibling;
		object->doWhileExtra->statements = temp;
		temp->nextSibling = nullptr;
		object->firstChild = nullptr;
	}

	void configureForObject(Object *object) {
		Object *temp = object->firstChild;

		object->forExtra = allocator.getMemory<ForExtra>();
		object->forExtra->statements = temp->nextSibling->nextSibling->nextSibling;
		object->forExtra->afterPart = temp->nextSibling->nextSibling;
		object->forExtra->afterPart->nextSibling = nullptr;
		object->forExtra->condPart = temp->nextSibling;
		object->forExtra->condPart->nextSibling = nullptr;
		object->forExtra->initPart = temp;
		object->forExtra->initPart->nextSibling = nullptr;

		object->firstChild = nullptr;
	}

	void configureIfObject(Object* object) {
		Object *temp = object->firstChild;
		object->ifExtra = allocator.getMemory<IfExtra>();

		object->ifExtra->cond = temp;

		object->ifExtra->ifPart = temp->nextSibling;

		Object** place = &(object->ifExtra->ifPart);
		object->ifExtra->elsePart = nullptr;
		while (*place != nullptr) {
			if ((*place)->objectType == Tt_else) {
				object->ifExtra->elsePart = (*place)->nextSibling;
				allocator.deleteMemory <Object>(*place);
				*place = nullptr;
				break;
			}
			place = &(*place)->nextSibling;
		}
		temp->nextSibling = nullptr;
		object->firstChild = nullptr;
	}

	void configureFuncDefObject(Object *object) {
		Object *temp = object->firstChild->nextSibling;
		object->funcDefExtra = allocator.getMemory<FuncDefExtra>();
		object->funcDefExtra->name = object->firstChild;
		object->funcDefExtra->name->nextSibling = nullptr;

		Object **place = &(object->funcDefExtra->parameters);
		while (temp->objectType != Tt_statement) {
			*place = temp;
			place = &((*place)->nextSibling);
			temp = temp->nextSibling;
		}
		*place = nullptr;
		object->funcDefExtra->func_body = temp;
		object->firstChild = nullptr;
	}


	void configureFuncCallObject(Object *object) {
		Object *temp = object->firstChild->nextSibling;
		object->funcCallExtra = allocator.getMemory<FuncCallExtra>();
		object->funcCallExtra->name = object->firstChild;
		object->funcCallExtra->name->nextSibling = nullptr;
		object->funcCallExtra->values = temp;
		object->firstChild = nullptr;
	}



	void configureValueObject(Object *object) {
		std::queue <Object*> newOrder;
		std::stack <Object*> opStack;

		Object *it = object->firstChild;
		Object **temp;

		while (it != NULL) {
			if (it->objectType != Tt_operator) {
				newOrder.push(it);
			} else {
				while (!opStack.empty() && operatorPrecedences[(int)opStack.top()->extra] > operatorPrecedences[(int)it->extra]) {
					newOrder.push(opStack.top());
					opStack.pop();
				}
				opStack.push(it);
			}
			it = it->nextSibling;
		}
		temp = &object->firstChild;
		while (!newOrder.empty()) {
			*temp = newOrder.front();
			temp = &((*temp)->nextSibling);
			newOrder.pop();
		}
		while (!opStack.empty()) {
			*temp = opStack.top();
			temp = &((*temp)->nextSibling);
			opStack.pop();
		}
		*temp = nullptr;
	}


	void configureObject(Object* object) {
		switch (object->objectType) {
			case Tt_whileloop:
				configureWhileObject(object);
				break;
			case Tt_dowhileloop:
				configureDoWhileObject(object);
				break;
			case Tt_ifcond:
				configureIfObject(object);
				break;
			case Tt_forloop:
				configureForObject(object);
				break;
			case Tt_funcdef:
				configureFuncDefObject(object);
				break;
			case Tt_funccall:
				configureFuncCallObject(object);
				break;
			case Tt_value:
				configureValueObject(object);
				break;
			default:
				break;
		}
	}

	Object* buildObject(Token& tokenData) {
		Object* object = allocator.getMemory<Object>();
		object->objectType = tokenData.tokenType;
		switch (object->objectType) {
			case Tt_integer:
				memcpy(strnull, tokenData.tokenBegin, tokenData.tokenLength);
				strnull[tokenData.tokenLength] = '\0';
				object->intVal = atoi(strnull);
				break;
			case Tt_float:
				memcpy(strnull, tokenData.tokenBegin, tokenData.tokenLength);
				strnull[tokenData.tokenLength] = '\0';
				object->floatVal = (float)atof(strnull);
				break;
			case Tt_string:
			case Tt_variable:
				object->pChar = allocator.getMemory<char>(tokenData.tokenLength + 1);
				memcpy(object->pChar, tokenData.tokenBegin, tokenData.tokenLength);
				object->pChar[tokenData.tokenLength] = '\0';
				break;
			case Tt_operator:
			case Tt_singleoperator:
			case Tt_unary:
				object->opType = getOperatorType(tokenData);
				break;
		}

		return object;
	}

public:

	Object * buildProgram(TokenList& tokenDataList) {
		Object *prevObject = nullptr;
		Object *firstObject = nullptr;
		for (unsigned i = 0; i < tokenDataList.size(); i++) {
			Object* object = buildObject(tokenDataList[i]);
			if (prevObject) {
				prevObject->nextSibling = object;
			}

			if (tokenDataList[i].children.size() > 0) {
				object->firstChild = buildProgram(tokenDataList[i].children);
			}

			configureObject(object);

			prevObject = object;
			if (i == 0) {
				firstObject = object;
			}
		}

		return firstObject;
	}

	void printProgram(Object* object, int level = 0) {
		if (object == nullptr) return;
		for (int i = 0; i < level * 2; i++) printf(" ");

		switch (object->objectType) {
			case Tt_integer:
				printf("%d", object->intVal);
				break;
			case Tt_float:
				printf("%f", object->floatVal);
				break;
			case Tt_string:
				printf("%s", object->pChar);
				break;
			case Tt_variable:
				printf("%s", object->pChar);
				break;
			case Tt_operator:
			case Tt_singleoperator:
			case Tt_unary:
				printf("%s", getOperatorString(object->opType));
				break;
			case Tt_ifcond:
				printf("if\n");
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" condition :\n");
				printProgram(object->ifExtra->cond, level + 1);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" if part :\n");
				printProgram(object->ifExtra->ifPart, level + 1);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" else part :\n");
				printProgram(object->ifExtra->elsePart, level + 1);
				break;

			case Tt_whileloop:
				printf("while\n");
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" condition :\n");
				printProgram(object->whileExtra->cond, level + 1);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" statements :\n");
				printProgram(object->whileExtra->statements, level + 1);
				break;
			case Tt_dowhileloop:
				printf("do while\n");
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" statements :\n");
				printProgram(object->doWhileExtra->statements, level + 1);

				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" condition :\n");
				printProgram(object->doWhileExtra->cond, level + 1);
				break;

			case Tt_forloop:
				printf("for\n");
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" init :\n");
				printProgram(object->forExtra->initPart, level + 1);

				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" condition :\n");
				printProgram(object->forExtra->condPart, level + 1);

				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" after :\n");
				printProgram(object->forExtra->afterPart, level + 1);

				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" statements :\n");
				printProgram(object->forExtra->statements, level + 1);

				break;

			default:
				printf("%s", getTokenName(object->objectType));
		}
		printf("\n");
		printProgram(object->firstChild, level + 1);
		printProgram(object->nextSibling, level);

	}
};


#endif 
