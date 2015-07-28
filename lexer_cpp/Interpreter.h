#pragma once

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Tools.h"

#define STACK_SIZE 1000
#define HEAP_SIZE 10000

class Interpreter {

	enum ValueType {
		Vt_integer,
		Vt_float,
		Vt_string,
		Vt_function
	};


	struct Value {
		ValueType valueType;
		union {
			int32_t intVal;
			float floatVal;
		};
	};

	bool isValueDynamic(const Value* value) {
		return value->valueType >= Vt_string;
	}

	char *allData;

	char *stack;
	char *heap;

	Value *valueStackPointer;
	Value *variableStackPointer;
	Value *variableGlobalPointer;
	DynamicAllocator allocator;

	void convert(Value *value, ValueType newType) {
		char temp[32];
		int l;
		switch (value->valueType) {
			case Vt_integer:
				switch (newType) {
					case Vt_float:
						value->floatVal = (float)value->intVal;
						break;
					case Vt_string:
						l = intToStr(value->intVal, temp, sizeof(temp));
						value->intVal = allocator.allocId(l + 1);
						memcpy(allocator.getAddress(value->intVal), temp, l + 1);
						break;
				}
				break;
			case Vt_float:
				switch (newType) {
					case Vt_integer:
						value->intVal = (int32_t)value->floatVal;
						break;
					case Vt_string:
						l = floatToStr(value->floatVal, temp, sizeof(temp));
						value->intVal = allocator.allocId(l + 1);
						memcpy(allocator.getAddress(value->intVal), temp, l + 1);
						break;
				}
				break;
			case Vt_string:
				// TODO
				break;
			default:
				break;

		}
		value->valueType = newType;
	}

	Value* interperetStringOperator(const Object *op, Value* v1, Value* v2) {
		int l1 = strlen((const char*)allocator.getAddress(v1->intVal));
		int l2 = strlen((const char*)allocator.getAddress(v2->intVal));
		uint32_t temp;
		char* c;
		switch (op->opType) {
			case Op_addition:
				temp = allocator.allocId(l1 + l2 + 1);
				c = (char*)allocator.getAddress(temp);
				memcpy(c, allocator.getAddress(v1->intVal), l1);
				memcpy(c + l1, allocator.getAddress(v2->intVal), l2);
				c[l1 + l2] = '\0';
				allocator.freeId(v1->intVal);
				v1->intVal = temp;
				break;
			default:
				printf("Undefined operator for string.\n");
				break;
		}
		return v1;
	}

	Value* interperetFloatOperator(const Object *op, Value* v1, Value* v2) {
		switch (op->opType) {
			case Op_addition:
				v1->floatVal += v2->floatVal;
				break;
			case Op_subtraction:
				v1->floatVal -= v2->floatVal;
				break;
			case Op_multiplication:
				v1->floatVal *= v2->floatVal;
				break;
			case Op_division:
				v1->floatVal /= v2->floatVal;
				break;
			case Op_equality:
				v1->valueType = Vt_integer;
				v1->intVal = v1->floatVal == v2->floatVal;
				break;
			case Op_inequality:
				v1->valueType = Vt_integer;
				v1->intVal = v1->floatVal != v2->floatVal;
				break;
			case Op_greater_than:
				v1->valueType = Vt_integer;
				v1->intVal = v1->floatVal > v2->floatVal;
				break;
			case Op_greater_or_equal:
				v1->valueType = Vt_integer;
				v1->intVal = v1->floatVal >= v2->floatVal;
				break;
			case Op_less_than:
				v1->valueType = Vt_integer;
				v1->intVal = v1->floatVal < v2->floatVal;
				break;
			case Op_less_or_equal:
				v1->valueType = Vt_integer;
				v1->intVal = v1->floatVal <= v2->floatVal;
				break;
			case Op_negate:
				v1 = v2;
				v2->floatVal = -v2->floatVal;
				break;
			case Op_plus:
				v1 = v2;
				break;
			default:
				printf("Undefined operator for float.\n");
				break;
		}
		return v1;
	}

	Value* interperetIntegerOperator(const Object *op, Value* v1, Value* v2) {
		switch (op->opType) {
			case Op_addition:
				v1->intVal += v2->intVal;
				break;
			case Op_subtraction:
				v1->intVal -= v2->intVal;
				break;
			case Op_multiplication:
				v1->intVal *= v2->intVal;
				break;
			case Op_division:
				v1->intVal /= v2->intVal;
				break;
			case Op_modula:
				v1->intVal %= v2->intVal;
				break;
			case Op_equality:
				v1->intVal = v1->floatVal == v2->floatVal;
				break;
			case Op_inequality:
				v1->intVal = v1->floatVal != v2->floatVal;
				break;
			case Op_greater_than:
				v1->intVal = v1->floatVal > v2->floatVal;
				break;
			case Op_greater_or_equal:
				v1->intVal = v1->floatVal >= v2->floatVal;
				break;
			case Op_less_than:
				v1->intVal = v1->floatVal < v2->floatVal;
				break;
			case Op_less_or_equal:
				v1->intVal = v1->floatVal <= v2->floatVal;
				break;
			case Op_negate:
				v1 = v2;
				v2->floatVal = -v2->floatVal;
				break;
			case Op_plus:
				v1 = v2;
				break;
			default:
				printf("Undefined operator for integer.\n");
				break;
		}
		return v1;
	}



	Value* interpretValue(const Object *object) {
		Value *v = nullptr, *vPrev = nullptr, *vPrev2 = nullptr;
		if (object == nullptr) return nullptr;
		Value* oldValueStackPointer = valueStackPointer;
		const Object** temp;
		Value* variable;
		switch (object->objectType) {
			default:
				printf("err\n");
				v = interpretValue(object->firstChild);
				break;
			case Tt_value:
				v = interpretValue(object->firstChild);
				break;
			case Tt_assignment:
				v = interpretValue(object->firstChild->nextSibling);

				variable = getVariableValue(object->firstChild->intVal);

				*variable = *v;

				if (isValueDynamic(variable)) {
					variable->intVal = allocator.getCopyId(variable->intVal);
				}

				break;
			case Tt_variable:
				v = valueStackPointer++;
				variable = getVariableValue(object->intVal);
				*v = *variable;

				if (isValueDynamic(v)) {
					v->intVal = allocator.getCopyId(v->intVal);
				}

				break;

			case Tt_variable_withpost:
				variable = getVariableValue(object->firstChild->intVal);
				if (isValueDynamic(variable)) {
					printf("Undefined operator.\n");
					break;
				}
				v = valueStackPointer++;
				*v = *variable;
				switch (object->firstChild->nextSibling->opType) {
					case Op_inc:
						switch (variable->valueType) {
							case Vt_integer:
								variable->intVal++;
								break;
							case Vt_float:
								variable->floatVal += 1.0f;
								break;
						}
						break;
					case Op_dec:
						switch (variable->valueType) {
							case Vt_integer:
								variable->intVal--;
								break;
							case Vt_float:
								variable->floatVal -= 1.0f;
								break;
						}
						break;
				}
				break;

			case Tt_variable_withpre:

				variable = getVariableValue(object->firstChild->nextSibling->intVal);

				if (isValueDynamic(variable)) {
					printf("Undefined operator.\n");
					break;
				}
				v = valueStackPointer++;
				switch (object->firstChild->opType) {
					case Op_inc:
						switch (variable->valueType) {
							case Vt_integer:
								variable->intVal++;
								break;
							case Vt_float:
								variable->floatVal += 1.0f;  // perfect
								break;
						}
						break;
					case Op_dec:
						switch (variable->valueType) {
							case Vt_integer:
								variable->intVal--;
								break;
							case Vt_float:
								variable->floatVal -= 1.0f;
								break;
						}
						break;
				}
				*v = *variable;
				break;
			case Tt_string:
				v = valueStackPointer++;
				v->valueType = Vt_string;
				v->intVal = allocator.allocId(strlen(object->pChar) + 1);
				memcpy(allocator.getAddress(v->intVal), object->pChar, strlen(object->pChar) + 1);
				break;
			case Tt_integer:
				v = valueStackPointer++;
				v->valueType = Vt_integer;
				v->intVal = object->intVal;
				break;
			case Tt_float:
				v = valueStackPointer++;
				v->valueType = Vt_float;
				v->floatVal = object->floatVal;
				break;


			case Tt_funcdef:
				v = valueStackPointer++;
				v->valueType = Vt_function;
				v->intVal = allocator.allocId<Object*>();
				temp = (const Object**)allocator.getAddress(v->intVal);
				*temp = object;
				break;
			case Tt_funccall:
				v = interpretValue(object->funcCallExtra->name);

				if(v->valueType == Vt_function){
					const Object* funcDef = *(const Object**)allocator.getAddress(v->intVal);

					Value* oldVariableStackPointer = variableStackPointer;
					variableStackPointer = valueStackPointer;
					memset(valueStackPointer, 0, sizeof(Value)*funcDef->funcDefExtra->parameterCount);

					const Object* p = object->funcCallExtra->values;
					while (p) {
						if (p->objectType == Tt_comma) {
							p = p->nextSibling;
							continue;
						}
						if (valueStackPointer >= variableStackPointer + funcDef->funcDefExtra->parameterCount) {
							printf("Too many parameters for function\n");
							break;
						}
						*valueStackPointer = *interpretValue(p);
						valueStackPointer++;
						p = p->nextSibling;
					}
					valueStackPointer = variableStackPointer + funcDef->funcDefExtra->parameterCount;
					int ret = interpretFlow(funcDef->funcDefExtra->func_body) - 3;
					valueStackPointer -= funcDef->funcDefExtra->parameterCount;
					variableStackPointer = oldVariableStackPointer;

					v = valueStackPointer;
					if(ret >= 0){
						*v = *(valueStackPointer + ret);

						if (isValueDynamic(v)) {
							v->intVal = allocator.getCopyId(v->intVal);
						}
					}else{
						v->valueType = Vt_integer;
						v->intVal = 0;
					}
				}else{
					printf("It is not a function");
				}
				break;
			case Tt_operator:
				vPrev = valueStackPointer - 1;
				if (!isOperatorUnion(object->opType)) {
					vPrev2 = valueStackPointer - 2;
					if (vPrev->valueType < vPrev2->valueType) {
						convert(vPrev, vPrev2->valueType);
					} else if (vPrev->valueType > vPrev2->valueType) {
						convert(vPrev2, vPrev->valueType);
					}
				}
				switch (vPrev->valueType) {
					case Vt_string:
						v = interperetStringOperator(object, vPrev2, vPrev);
						break;
					case Vt_float:
						v = interperetFloatOperator(object, vPrev2, vPrev);
						break;
					case Vt_integer:
						v = interperetIntegerOperator(object, vPrev2, vPrev);
						break;
				}

				if (!isOperatorUnion(object->opType)) {
					valueStackPointer--;
					if (isValueDynamic(valueStackPointer)) {
						allocator.freeId(valueStackPointer->intVal);
					}
				}

				break;
		}
		if (object->nextSibling) {
			if (object->nextSibling->objectType > Tt_valuetypes_start && object->nextSibling->objectType < Tt_valuetypes_end) {
				vPrev2 = interpretValue(object->nextSibling);
			}
		}

		valueStackPointer = oldValueStackPointer;
		return vPrev2 ? vPrev2 : v;
	}

	bool valueToBool(Value* v) {
		switch (v->valueType) {
			case Vt_integer:
				return v->intVal != 0;
			case Vt_float:
				return v->floatVal != 0.0f;
			case Vt_string:
				// TODO
				break;
		}
		return 0;
	}

	int interpretFlow(const Object *object, bool goNext = true) {
		if (object == nullptr) return 0;

		int result = 0;
		switch (object->objectType) {
			default:
				result = interpretFlow(object->firstChild);
				break;
			case Tt_multiple_statement:
				memset(valueStackPointer, 0, sizeof(Value)*object->intVal); // new variables

				valueStackPointer += object->intVal;
				result = interpretFlow(object->firstChild);
				valueStackPointer -= object->intVal;
				break;
			case Tt_program:
				memset(valueStackPointer, 0, sizeof(Value)*object->intVal); // new variables

				valueStackPointer += object->intVal;
				result = interpretFlow(object->firstChild);
				break;
			case Tt_statement:
				result = interpretFlow(object->firstChild);
				break;
			case Tt_return:
				return (interpretValue(object->firstChild) - variableStackPointer) + 3;
				break;
			case Tt_break:
				return 2;
				break;
			case Tt_whileloop:
				memset(valueStackPointer, 0, sizeof(Value)*object->whileExtra->parameterCount); // new variables

				valueStackPointer += object->whileExtra->parameterCount;
				while (valueToBool(interpretValue(object->whileExtra->cond))) {
					int bodyResult = 0;
					if(bodyResult = interpretFlow(object->whileExtra->statements) >= 2)
					{
						if(bodyResult >= 3){
							result = bodyResult;
						}
						break;
					}
				}
				valueStackPointer -= object->whileExtra->parameterCount;
				break;
			case Tt_dowhileloop:
				memset(valueStackPointer, 0, sizeof(Value)*object->doWhileExtra->parameterCount); // new variables

				valueStackPointer += object->doWhileExtra->parameterCount;
				do {
					int bodyResult = 0;

					if(bodyResult = interpretFlow(object->doWhileExtra->statements) >= 2)
					{
						if(bodyResult >= 3){
							result = bodyResult;
						}
						break;
					}
				} while (valueToBool(interpretValue(object->doWhileExtra->cond)));
				valueStackPointer -= object->doWhileExtra->parameterCount;
				break;
			case Tt_ifcond:
				memset(valueStackPointer, 0, sizeof(Value)*object->ifExtra->parameterCount); // new variables

				valueStackPointer += object->ifExtra->parameterCount;
				if (valueToBool(interpretValue(object->ifExtra->cond))) {
					result = interpretFlow(object->ifExtra->ifPart);
				} else {
					result = interpretFlow(object->ifExtra->elsePart);
				}
				valueStackPointer -= object->ifExtra->parameterCount;
				break;
			case Tt_forloop:
				memset(valueStackPointer, 0, sizeof(Value)*object->forExtra->parameterCount); // new variables

				valueStackPointer += object->forExtra->parameterCount;
				for (interpretValue(object->forExtra->initPart); valueToBool(interpretValue(object->forExtra->condPart)); interpretValue(object->forExtra->afterPart)) {
					int bodyResult = 0;
					if (bodyResult = interpretFlow(object->forExtra->statements) >= 2){
						if(bodyResult >= 3){
							result = bodyResult;
						}
						break;
					}
				}
				valueStackPointer -= object->forExtra->parameterCount;
				break;
			case Tt_value:
				interpretValue(object->firstChild);
				if (isValueDynamic(valueStackPointer)) {
					allocator.freeId(valueStackPointer->intVal);
				}
				break;
		}
		if (result) return result;
		if (goNext) {
			return interpretFlow(object->nextSibling);
		}
		return 0;
	}


public:
	Interpreter() {
		allData = new char[STACK_SIZE + HEAP_SIZE];
		stack = allData;
		heap = allData + STACK_SIZE;
		allocator.setBuffer(heap, HEAP_SIZE);

		variableStackPointer = (Value*)stack;
		valueStackPointer = variableStackPointer;
		variableGlobalPointer = variableStackPointer;
	}

	~Interpreter() {
		delete allData;
	}

	void interprete(const Object* program) {
		interpretFlow(program);
	}

	void printVal(const ProgramBuilder& builder, const std::string& varName) {
		for (int i = 0; i < builder.globalCount; i++) {
			if (builder.varNames[i] == varName) {
				Value* v = &variableStackPointer[i];
				switch (v->valueType) {
					case Vt_integer:
						printf("%s = %d\n", varName.c_str(), v->intVal);
						break;
					case Vt_float:
						printf("%s = %f\n", varName.c_str(), v->floatVal);
						break;
					case Vt_string:
						printf("%s = %s\n", varName.c_str(), (char*)allocator.getAddress(v->intVal));
						break;
					case Vt_function:
						printf("%s = func\n", varName.c_str());
						break;
				}
				return;
			}
		}
		printf("There is no global named '%s'\n", varName.c_str());
	}

	Value* getVariableValue(int32_t intVal) {

		if (intVal >= 0) {
			return &variableStackPointer[intVal];
		} else {
			return &variableGlobalPointer[-1 - intVal];
		}
	}

	void printStackTop(const ProgramBuilder& builder)
	{
		Value* v = valueStackPointer;
		switch (v->valueType) {
		case Vt_integer:
			printf("%d\n", v->intVal);
			break;
		case Vt_float:
			printf("%f\n", v->floatVal);
			break;
		case Vt_string:
			printf("%s\n", (char*)allocator.getAddress(v->intVal));
			break;
		case Vt_function:
			builder.printObject(*(const Object**)allocator.getAddress(v->intVal));
			break;
		}
		return;
	}



};

#endif
