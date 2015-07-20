#pragma once

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Tools.h"

#define STACK_SIZE 10000

class Interpreter {
	struct Value {
		TokenType valueType;
		union {
			int32_t intVal;
			float floatVal;
		};
	};

	char *stack;
	char *heapEnd;
	Value *valueStackPointer;
	Value *variableStackPointer;


	void convert(Value *value, TokenType newType) {
		switch (value->valueType) {
			case Tt_integer:
				switch (newType) {
					case Tt_float:
						value->floatVal = (float)value->intVal;
						break;
					case Tt_string:
						// TODO
						break;
				}
				break;
			case Tt_float:
				switch (newType) {
					case Tt_integer:
						value->intVal = (int32_t)value->floatVal;
						break;
					case Tt_string:
						// TODO
						break;
				}
				break;
			case Tt_string:
				// TODO
				break;
			default:
				break;

		}
		value->valueType = newType;
	}

	Value* interpereteFloatOperator(Object *op, Value* v1, Value* v2) {
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
				v1->valueType = Tt_integer;
				v1->intVal = v1->floatVal == v2->floatVal;
				break;
			case Op_inequality:
				v1->valueType = Tt_integer;
				v1->intVal = v1->floatVal != v2->floatVal;
				break;
			case Op_greater_than:
				v1->valueType = Tt_integer;
				v1->intVal = v1->floatVal > v2->floatVal;
				break;
			case Op_greater_or_equal:
				v1->valueType = Tt_integer;
				v1->intVal = v1->floatVal >= v2->floatVal;
				break;
			case Op_less_than:
				v1->valueType = Tt_integer;
				v1->intVal = v1->floatVal < v2->floatVal;
				break;
			case Op_less_or_equal:
				v1->valueType = Tt_integer;
				v1->intVal = v1->floatVal <= v2->floatVal;
				break;
			case Op_negate:
				v1 = v2;
				v2->floatVal = -v2->floatVal;
				break;
			case Op_plus:
				v1 = v2;
				break;
		}
		return v1;
	}

	Value* interpereteIntegerOperator(Object *op, Value* v1, Value* v2) {
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
		}
		return v1;
	}



	Value* interpretValue(Object *object) {
		Value *v = nullptr, *vPrev = nullptr, *vPrev2 = nullptr;
		if (object == nullptr) return nullptr;
		Value* oldValueStackPointer = valueStackPointer;

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
				variableStackPointer[object->firstChild->intVal] = *v;
				break;
			case Tt_variable:
				v = valueStackPointer++;
				*v = variableStackPointer[object->intVal];
				break;

			case Tt_variable_withpost:
				v = valueStackPointer++;
				*v = variableStackPointer[object->firstChild->intVal];
				switch (object->firstChild->nextSibling->opType) {
					case Op_inc:
						switch (variableStackPointer[object->firstChild->intVal].valueType) {
							case Tt_integer:
								variableStackPointer[object->firstChild->intVal].intVal++;
								break;
							case Tt_float:
								variableStackPointer[object->firstChild->intVal].floatVal += 1.0f;
								break;
						}
						break;
					case Op_dec:
						switch (variableStackPointer[object->firstChild->intVal].valueType) {
							case Tt_integer:
								variableStackPointer[object->firstChild->intVal].intVal--;
								break;
							case Tt_float:
								variableStackPointer[object->firstChild->intVal].floatVal -= 1.0f;
								break;
						}
						break;
				}
				break;

			case Tt_variable_withpre:
				v = valueStackPointer++;
				switch (object->firstChild->opType) {
					case Op_inc:
						switch (variableStackPointer[object->firstChild->nextSibling->intVal].valueType) {
							case Tt_integer:
								variableStackPointer[object->firstChild->nextSibling->intVal].intVal++;
								break;
							case Tt_float:
								variableStackPointer[object->firstChild->nextSibling->intVal].floatVal += 1.0f;  // perfect
								break;
						}
						break;
					case Op_dec:
						switch (variableStackPointer[object->firstChild->nextSibling->intVal].valueType) {
							case Tt_integer:
								variableStackPointer[object->firstChild->nextSibling->intVal].intVal--;
								break;
							case Tt_float:
								variableStackPointer[object->firstChild->nextSibling->intVal].floatVal -= 1.0f;
								break;
						}
						break;
				}
				*v = variableStackPointer[object->firstChild->nextSibling->intVal];
				break;
			case Tt_string:
				// TODO
				break;
			case Tt_integer:
				v = valueStackPointer++;
				v->valueType = Tt_integer;
				v->intVal = object->intVal;
				break;
			case Tt_float:
				v = valueStackPointer++;
				v->valueType = Tt_float;
				v->floatVal = object->floatVal;
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
					case Tt_string:
						// TODO
						break;
					case Tt_float:
						v = interpereteFloatOperator(object, vPrev2, vPrev);
						break;
					case Tt_integer:
						v = interpereteIntegerOperator(object, vPrev2, vPrev);
						break;
				}

				if (!isOperatorUnion(object->opType))
					valueStackPointer--;

				break;

			case Tt_funcdef:
				// TODO
				break;
			case Tt_funccall:
				// TODO
				break;
		}
		vPrev2 = interpretValue(object->nextSibling);
		valueStackPointer = oldValueStackPointer;
		return vPrev2 ? vPrev2 : v;
	}

	bool valueToBool(Value* v) {
		switch (v->valueType) {
			case Tt_integer:
				return v->intVal != 0;
			case Tt_float:
				return v->floatVal != 0.0f;
			case Tt_string:
				// TODO
				break;
		}
		return 0;
	}

	int interpretFlow(Object *object) {
		if (object == nullptr) return 0;

		int result = 0;
		switch (object->objectType) {
			default:
				result = interpretFlow(object->firstChild);
				break;
			case Tt_multiple_statement:
			case Tt_program:
				memset(valueStackPointer, 0, sizeof(Object)*object->intVal); // new variables

				valueStackPointer += object->intVal;
				result = interpretFlow(object->firstChild);
				valueStackPointer -= object->intVal;
				break;
			case Tt_statement:
				result = interpretFlow(object->firstChild);
				break;
			case Tt_return:
				interpretFlow(object->firstChild);
				return true;
				break;
			case Tt_break:
				return true;
				break;
			case Tt_whileloop:
				memset(valueStackPointer, 0, sizeof(Object)*object->whileExtra->scopeSize); // new variables

				valueStackPointer += object->whileExtra->scopeSize;
				while (valueToBool(interpretValue(object->whileExtra->cond))) {
					interpretFlow(object->whileExtra->statements);
				}
				valueStackPointer -= object->whileExtra->scopeSize;
				break;
			case Tt_dowhileloop:
				memset(valueStackPointer, 0, sizeof(Object)*object->doWhileExtra->scopeSize); // new variables

				valueStackPointer += object->doWhileExtra->scopeSize;
				do {
					interpretFlow(object->doWhileExtra->statements);
				} while (valueToBool(interpretValue(object->doWhileExtra->cond)));
				valueStackPointer -= object->doWhileExtra->scopeSize;
				break;
			case Tt_ifcond:
				memset(valueStackPointer, 0, sizeof(Object)*object->ifExtra->scopeSize); // new variables

				valueStackPointer += object->ifExtra->scopeSize;
				if (valueToBool(interpretValue(object->ifExtra->cond))) {
					result = interpretFlow(object->ifExtra->ifPart);
				} else {
					result = interpretFlow(object->ifExtra->elsePart);
				}
				valueStackPointer -= object->ifExtra->scopeSize;
				break;
			case Tt_forloop:
				memset(valueStackPointer, 0, sizeof(Object)*object->forExtra->scopeSize); // new variables

				valueStackPointer += object->forExtra->scopeSize;
				for (interpretValue(object->forExtra->initPart); valueToBool(interpretValue(object->forExtra->condPart)); interpretValue(object->forExtra->afterPart)) {
					if (interpretFlow(object->forExtra->statements)) break;
				}
				valueStackPointer -= object->forExtra->scopeSize;
				break;
			case Tt_funcdef:
			case Tt_funccall:
			case Tt_value:
			case Tt_assignment:
				interpretValue(object->firstChild);
				break;
		}
		if (result) return result;
		return interpretFlow(object->nextSibling);
	}


public:
	Interpreter() {
		stack = new char[STACK_SIZE];
		valueStackPointer = (Value*)stack;
		variableStackPointer = valueStackPointer;
		heapEnd = stack + STACK_SIZE;
	}

	~Interpreter() {
		delete stack;
	}

	void interprete(const Program& program) {
		valueStackPointer = (Value*)stack;
		variableStackPointer = valueStackPointer;
		heapEnd = stack + STACK_SIZE;
		interpretFlow(program.root);
	}

	void printVal(const Program& program, const std::string& varName) {
		for (unsigned i = 0; i < program.globals.size(); i++) {
			if (program.globals[i] == varName) {
				Value* v = &variableStackPointer[i];
				switch (v->valueType) {
					case Tt_integer:
						printf("%s = %d\n", varName.c_str(), v->intVal);
						break;
					case Tt_float:
						printf("%s = %f\n", varName.c_str(), v->floatVal);
						break;
					case Tt_string:
						// TODO
						break;
				}
				return;
			}
		}
		printf("There is no global named '%s'\n", varName.c_str());
	}
};

#endif