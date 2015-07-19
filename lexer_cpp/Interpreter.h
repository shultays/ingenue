#pragma once

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Tools.h"

#define STACK_SIZE 10000

class Interpreter {
	struct Value {
		TokenType valueType;
		union {
			int intVal;
			float floatVal;
		};
	};

	char *stack;
	char *heapEnd;
	Value *valueStackPointer;

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
						value->intVal = (int)value->floatVal;
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
		bool temp;

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
				temp = v1->floatVal == v2->floatVal;
				convert(v1, Tt_integer);
				v1->intVal = temp;
				break;
			case Op_inequality:
				temp = v1->floatVal != v2->floatVal;
				convert(v1, Tt_integer);
				v1->intVal = temp;
				break;
			case Op_greater_than:
				temp = v1->floatVal > v2->floatVal;
				convert(v1, Tt_integer);
				v1->intVal = temp;
				break;
			case Op_greater_or_equal:
				temp = v1->floatVal >= v2->floatVal;
				convert(v1, Tt_integer);
				v1->intVal = temp;
				break;
			case Op_less_than:
				temp = v1->floatVal < v2->floatVal;
				convert(v1, Tt_integer);
				v1->intVal = temp;
				break;
			case Op_less_or_equal:
				temp = v1->floatVal <= v2->floatVal;
				convert(v1, Tt_integer);
				v1->intVal = temp;
				break;
			case Op_pre_dec:
			case Op_pre_inc:
			case Op_post_dec:
			case Op_post_inc:
				// TODO
				break;
			case Op_negate:
				v1 = v2;
				v2->floatVal = -v2->floatVal;
				break;
			case Op_plus:
				v1 = v2;
				break;
		}
		return v2;
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
			case Op_pre_dec:
			case Op_pre_inc:
			case Op_post_dec:
			case Op_post_inc:
				// TODO
				break;
			case Op_negate:
				v1 = v2;
				v2->floatVal = -v2->floatVal;
				break;
			case Op_plus:
				v1 = v2;
				break;
		}
		return v2;
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
				interpretValue(object->firstChild->nextSibling);
				// TODO
				break;
			case Tt_variable:
				// TODO
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
				while (valueToBool(interpretValue(object->whileExtra->cond))) {
					interpretFlow(object->whileExtra->statements);
				}
				break;
			case Tt_dowhileloop:
				do {
					interpretFlow(object->doWhileExtra->statements);
				} while (valueToBool(interpretValue(object->doWhileExtra->cond)));
				break;
			case Tt_ifcond:
				if (valueToBool(interpretValue(object->ifExtra->cond))) {
					result = interpretFlow(object->ifExtra->ifPart);
				} else {
					result = interpretFlow(object->ifExtra->elsePart);
				}
				break;
			case Tt_forloop:
				for (interpretValue(object->forExtra->initPart); valueToBool(interpretValue(object->forExtra->condPart)); interpretValue(object->forExtra->afterPart)) {
					if (interpretFlow(object->forExtra->statements)) break;
				}
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
		int stackSize = 1024 * 1024 * 4;
		stack = new char[stackSize];
		valueStackPointer = (Value*)stack;
		heapEnd = stack + stackSize;
	}

	void interprete(Object* object) {
		interpretFlow(object);
	}

};

#endif