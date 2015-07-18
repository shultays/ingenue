#ifndef TOOLS_H
#define TOOLS_H

#include <vector>

enum TokenType {
	Tt_whitespace,
	Tt_comment_single,
	Tt_comment_multi,
	Tt_ws,
	Tt_empty,
	Tt_integer,
	Tt_float,
	Tt_operator,
	Tt_semicolon,
	Tt_if,
	Tt_p_left,
	Tt_p_right,
	Tt_cp_left,
	Tt_cp_right,
	Tt_for,
	Tt_while,
	Tt_do,
	Tt_string,
	Tt_variable,
	Tt_singleoperator,
	Tt_variable_withpost,
	Tt_variable_withpre,
	Tt_value,
	Tt_value_one,
	Tt_value_with_unary,
	Tt_unary,
	Tt_assign,
	Tt_assignment,
	Tt_funccall,
	Tt_statement,
	Tt_multiple_statement,
	Tt_return,
	Tt_break,
	Tt_ifcond,
	Tt_forloop,
	Tt_whileloop,
	Tt_dowhileloop,
	Tt_funcdef,
	Tt_else,
	Tt_program,

	Tt_invalid,

	Tt_count,
};

enum OperatorType {
	Op_err,
	Op_addition,
	Op_subtraction,
	Op_multiplication,
	Op_division,
	Op_modula,

	Op_equality,
	Op_inequality,
	Op_greater_than,
	Op_greater_or_equal,
	Op_less_than,
	Op_less_or_equal,

	Op_inc,
	Op_dec,

	Op_pre_inc,
	Op_post_inc,
	Op_pre_dec,
	Op_post_dec,

	Op_negate,
	Op_plus,
	Op_not,


	Op_count
};


class Token;
typedef std::vector<Token> TokenList;

class Token {
public:
	TokenType tokenType;

	const char *tokenBegin;
	int tokenLength;

	TokenList children;

	Token() {
		tokenType = Tt_invalid;
		tokenBegin = nullptr;
		tokenLength = -1;
	}
	Token(TokenType tokenType, const char *tokenBegin, int tokenLength = 0) {
		this->tokenType = tokenType;
		this->tokenBegin = tokenBegin;
		this->tokenLength = tokenLength;
	}
};



class Object;

typedef struct {
	Object *cond;
	Object *ifPart;
	Object *elsePart;
}IfExtra;


typedef struct {
	Object *initPart;
	Object *condPart;
	Object *afterPart;
	Object *statements;
}ForExtra;

typedef struct {
	Object *cond;
	Object *statements;
}WhileExtra, DoWhileExtra;


typedef struct {
	Object *name;
	Object *parameters;
	Object *func_body;
}FuncDefExtra;


typedef struct {
	Object *name;
	Object *values;
}FuncCallExtra;


class Object {
public:
	TokenType objectType;

	Object() {
		nextSibling = firstChild = nullptr;
	}

	Object(TokenType objectType) {
		this->objectType = objectType;
		nextSibling = firstChild = nullptr;
	}

	Object* nextSibling;
	Object* firstChild;

	union {
		void* extra;
		int intVal;
		float floatVal;
		char* pChar;

		OperatorType opType;

		IfExtra *ifExtra;
		ForExtra *forExtra;
		WhileExtra *whileExtra;
		DoWhileExtra *doWhileExtra;
		FuncDefExtra *funcDefExtra;
		FuncCallExtra *funcCallExtra;
	};
};

class MemoryAllocator {
public:
	template<class T>
	T* getMemory() {
		return getMemory<T>(1);
	}

	template<class T>
	void deleteMemory(T* object) {
		delete object;
	}

	template<class T>
	T* getMemory(int count) {
		return new T[count];
	}
};

void buildTools();

const char* getTokenName(TokenType type);
const char* getOperatorString(int type);

#endif