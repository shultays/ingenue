#ifndef PROGRAM_BUILDER_H
#define PROGRAM_BUILDER_H

#include <queue>
#include <stack>
#include <map>
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

	11,
	11,
	11,

	-1
};

class ProgramBuilder {
	friend class Interpreter;

	MemoryAllocator allocator;

	std::vector<std::string> varNames;
	int globalCount;

	std::map < const Object*, std::string> allNames; // debug

	std::stack<uint32_t> scopeSizeStack;
	std::stack<uint32_t> varNameCheckStartStack;
	
	int ignoreGlobals;

	int getVariableIndex(std::string varName, Object* object) {
		allNames[object] = varName;
		for (unsigned i = varNameCheckStartStack.top(); i < varNames.size(); i++) {
			if (varNames[i] == varName) return ((int)i) - varNameCheckStartStack.top();
		}

		if(ignoreGlobals == 0)
		{
			for (int i = 0; i < globalCount; i++) {
				if (varNames[i] == varName) return -((int)i) - 1;
			}
		}

		varNames.push_back(varName);
		if (scopeSizeStack.size() == 1) {
			globalCount++;
		}

		return varNames.size() - 1 - varNameCheckStartStack.top();
	}

	static OperatorType getOperatorType(Token& tokenData, bool isUnary) {
		int op = 0;
		if (isUnary) {
			op = Op_negate;
		}
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
		object->whileExtra->parameterCount = varNames.size() - scopeSizeStack.top();
		varNames.resize(scopeSizeStack.top());
		scopeSizeStack.pop();
		temp->nextSibling = nullptr;
		object->firstChild = nullptr;
	}

	void configureDoWhileObject(Object* object) {
		Object *temp = object->firstChild;
		object->doWhileExtra = allocator.getMemory<DoWhileExtra>();
		object->doWhileExtra->cond = temp->nextSibling;
		object->doWhileExtra->statements = temp;
		object->doWhileExtra->parameterCount = varNames.size() - scopeSizeStack.top();
		varNames.resize(scopeSizeStack.top());
		scopeSizeStack.pop();
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
		object->forExtra->parameterCount = varNames.size() - scopeSizeStack.top();
		varNames.resize(scopeSizeStack.top());
		scopeSizeStack.pop();
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

		object->ifExtra->parameterCount = varNames.size() - scopeSizeStack.top();
		varNames.resize(scopeSizeStack.top());
		scopeSizeStack.pop();
	}

	void configureFuncDefObject(Object *object) {
		Object *temp = object->firstChild;
		object->funcDefExtra = allocator.getMemory<FuncDefExtra>();
		Object **place = &(object->funcDefExtra->parameters);

		int parameterCount = 0;
		while (temp->objectType != Tt_multiple_statement) {
			*place = temp;
			place = &((*place)->nextSibling);
			temp = temp->nextSibling;
			parameterCount++;
		}
		*place = nullptr;
		object->funcDefExtra->func_body = temp;
		object->firstChild = nullptr;

		object->funcDefExtra->parameterCount = parameterCount;
		varNames.resize(scopeSizeStack.top());
		scopeSizeStack.pop();

		varNameCheckStartStack.pop();
	}


	void configureFuncCallObject(Object *object) {
		Object *temp = object->firstChild->nextSibling;
		object->funcCallExtra = allocator.getMemory<FuncCallExtra>();
		object->funcCallExtra->name = object->firstChild;
		object->funcCallExtra->name->nextSibling = nullptr;
		object->funcCallExtra->values = temp;
		object->firstChild = nullptr;
	}

	void configureUnaryObject(Object *object) {
		object->objectType = Tt_operator;
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
				while (!opStack.empty() && operatorPrecedences[opStack.top()->opType] > operatorPrecedences[it->opType]) {
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
			case Tt_func_def:
				configureFuncDefObject(object);
				break;
			case Tt_func_call:
				configureFuncCallObject(object);
				break;
			case Tt_value:
				configureValueObject(object);
				break;
			case Tt_unary:
				configureUnaryObject(object);
				break;
			case Tt_multiple_statement:
				object->intVal = varNames.size() - scopeSizeStack.top();
				varNames.resize(scopeSizeStack.top());
				scopeSizeStack.pop();
				break;
			case Tt_program:
				object->intVal = varNames.size() - scopeSizeStack.top();
				break;
			case Tt_variable_def:
				ignoreGlobals--;
				break;
			default:
				break;
		}
	}

	Object* buildObject(Token& tokenData) {
		char strnull[128];

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
				object->pChar = allocator.getMemory<char>(tokenData.tokenLength - 1);
				memcpy(object->pChar, tokenData.tokenBegin + 1, tokenData.tokenLength - 2);
				object->pChar[tokenData.tokenLength - 2] = '\0';
				break;
			case Tt_variable:
				memcpy(strnull, tokenData.tokenBegin, tokenData.tokenLength);
				strnull[tokenData.tokenLength] = '\0';
				object->intVal = getVariableIndex(strnull, object);
				break;
			case Tt_operator:
			case Tt_singleoperator:
			case Tt_unary:
				object->opType = getOperatorType(tokenData, object->objectType == Tt_unary);
				break;
			case Tt_multiple_statement:
			case Tt_program:
			case Tt_forloop:
			case Tt_whileloop:
			case Tt_dowhileloop:
			case Tt_ifcond:
				scopeSizeStack.push(varNames.size());
				break;
			case Tt_func_def:
				scopeSizeStack.push(varNames.size());
				varNameCheckStartStack.push(varNames.size());
				break;
			case Tt_variable_def:
				ignoreGlobals++;
				break;
		}

		return object;
	}



	Object * buildSubProgram(TokenList& tokenDataList) {
		Object *prevObject = nullptr;
		Object *firstObject = nullptr;
		for (unsigned i = 0; i < tokenDataList.size(); i++) {
			Object* object = buildObject(tokenDataList[i]);
			if (tokenDataList[i].children.size() > 0) {
				object->firstChild = buildSubProgram(tokenDataList[i].children);
			}

			configureObject(object);

			if (prevObject) {
				prevObject->nextSibling = object;
			}
			if (i == 0) {
				firstObject = object;
			}
			while (object->nextSibling) object = object->nextSibling;
			prevObject = object;
		}

		return firstObject;
	}

	void printObject(const Object* object, int level = 0) const {
		const Object* temp;
		if (object == nullptr) return;
		for (int i = 0; i < level * 2; i++) printf(" ");
		bool moveVariableStack = false;
		switch (object->objectType) {
			case Tt_program:
				printf("program %d", object->intVal);
				break;
			case Tt_multiple_statement:
				printf("multiple_statement %d", object->intVal);
				break;
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
				printf("%s (%d)", allNames.at(object).c_str(), object->intVal);
				break;
			case Tt_operator:
			case Tt_singleoperator:
				printf("%s %s", getOperatorString(object->opType), object->opType >= Op_negate ? " (unary)" : "");
				break;
			case Tt_ifcond:
				printf("if %d\n", object->ifExtra->parameterCount);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" condition :\n");
				printObject(object->ifExtra->cond, level + 1);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" if part :\n");
				printObject(object->ifExtra->ifPart, level + 1);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" else part :\n");
				printObject(object->ifExtra->elsePart, level + 1);
				break;

			case Tt_whileloop:
				printf("while %d\n", object->whileExtra->parameterCount);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" condition :\n");
				printObject(object->whileExtra->cond, level + 1);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" statements :\n");
				printObject(object->whileExtra->statements, level + 1);
				break;
			case Tt_dowhileloop:
				printf("do while %d\n", object->doWhileExtra->parameterCount);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" statements :\n");
				printObject(object->doWhileExtra->statements, level + 1);

				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" condition :\n");
				printObject(object->doWhileExtra->cond, level + 1);
				break;

			case Tt_forloop:
				printf("for %d\n", object->forExtra->parameterCount);
				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" init :\n");
				printObject(object->forExtra->initPart, level + 1);

				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" condition :\n");
				printObject(object->forExtra->condPart, level + 1);

				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" after :\n");
				printObject(object->forExtra->afterPart, level + 1);

				for (int i = 0; i < level * 2; i++) printf(" ");
				printf(" statements :\n");
				printObject(object->forExtra->statements, level + 1);

				break;

			case Tt_func_def:
				printf("func (");
				temp = object->funcDefExtra->parameters;
				while (temp) {
					printf("%s (%d)", allNames.at(temp->firstChild).c_str(), temp->intVal);
					temp = temp->nextSibling;
					if (temp) printf(", ");
				}
				printf(")\n");
				printObject(object->funcDefExtra->func_body, level + 1);
				break;
			default:
				printf("%s", getTokenName(object->objectType));
		}
		printf("\n");
		printObject(object->firstChild, level + 1);
		printObject(object->nextSibling, level);
	}

	void deleteObject(Object* object) {
		if (object == nullptr) return;

		switch (object->objectType) {
		case Tt_whileloop:
			deleteObject(object->whileExtra->cond);
			deleteObject(object->whileExtra->statements);
			allocator.deleteMemory<WhileExtra>(object->whileExtra);
			break;
		case Tt_dowhileloop:
			deleteObject(object->doWhileExtra->cond);
			deleteObject(object->doWhileExtra->statements);
			allocator.deleteMemory<DoWhileExtra>(object->doWhileExtra);
			break;
		case Tt_ifcond:
			deleteObject(object->ifExtra->cond);
			deleteObject(object->ifExtra->ifPart);
			deleteObject(object->ifExtra->elsePart);
			allocator.deleteMemory<IfExtra>(object->ifExtra);
			break;
		case Tt_forloop:
			deleteObject(object->forExtra->initPart);
			deleteObject(object->forExtra->condPart);
			deleteObject(object->forExtra->afterPart);
			deleteObject(object->forExtra->statements);
			allocator.deleteMemory<ForExtra>(object->forExtra);
			break;
		case Tt_func_def:
			deleteObject(object->funcDefExtra->parameters);
			deleteObject(object->funcDefExtra->func_body);
			allocator.deleteMemory<FuncDefExtra>(object->funcDefExtra);
			break;
		case Tt_func_call:
			deleteObject(object->funcCallExtra->values);
			allocator.deleteMemory<FuncCallExtra>(object->funcCallExtra);
			break;
		case Tt_string:
			allocator.deleteMemory<char>(object->pChar, strlen(object->pChar) + 1);
			break;

		default:
			break;
		}

		deleteObject(object->nextSibling);
		deleteObject(object->firstChild);

		allocator.deleteMemory<Object>(object);
	}
public:
	ProgramBuilder() {
		varNameCheckStartStack.push(0);
		globalCount = 0;
		ignoreGlobals = 0;
	}

	Object* buildProgram(TokenList& tokenDataList) {
		Object *object = buildSubProgram(tokenDataList);
		return object;
	}

	void printProgram(const Object* program) const
	{
		printObject(program, 1);
	}


	void deleteProgram(Object* program){
		deleteObject(program);
	}
};


#endif 
