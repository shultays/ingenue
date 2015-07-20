#ifndef TOOLS_H
#define TOOLS_H

#include <vector>
#include <cstdint>

enum TokenType {
	Tt_integer,
	Tt_float,
	Tt_string,
	Tt_whitespace,
	Tt_comment_single,
	Tt_comment_multi,
	Tt_ws,
	Tt_empty,
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
	Tt_variable,
	Tt_singleoperator,
	Tt_variable_withpost,
	Tt_variable_withpre,
	Tt_value,
	Tt_value_one,
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

	Op_negate,
	Op_plus,


	Op_count
};


bool isOperatorUnion(OperatorType op);

class Token;
typedef std::vector<Token> TokenList;

class Token {
public:
	TokenType tokenType;

	const char *tokenBegin;
	uint32_t tokenLength;

	TokenList children;

	Token() {
		tokenType = Tt_invalid;
		tokenBegin = nullptr;
		tokenLength = -1;
	}
	Token(TokenType tokenType, const char *tokenBegin, uint32_t tokenLength = 0) {
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
	uint32_t scopeSize;
}IfExtra;


typedef struct {
	Object *initPart;
	Object *condPart;
	Object *afterPart;
	Object *statements;

	uint32_t scopeSize;
}ForExtra;

typedef struct {
	Object *cond;
	Object *statements;

	uint32_t scopeSize;
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
		int32_t intVal;
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

class Program {
public:
	Object *root;
	std::vector<std::string> globals;
};

class MemoryAllocator {
public:
	uint32_t allocSize;

	MemoryAllocator() {
		allocSize = 0;
	}

	template<class T>
	T* getMemory() {
		allocSize += sizeof(T);
		return new T;
	}

	template<class T>
	void deleteMemory(T* object) {
		allocSize -= sizeof(T);
		delete object;
	}

	template<class T>
	T* getMemory(int count) {
		allocSize += sizeof(T)* count;
		return new T[count];
	}

	template<class T>
	void deleteMemory(T* object, int count) {
		allocSize -= sizeof(T)* count;
		delete[] object;
	}

};

void buildTools();

const char* getTokenName(TokenType type);
const char* getOperatorString(int type);


class DynamicAllocator {
public:
	static const uint32_t buffSize = 1000;

	struct Header {
		uint32_t isAllocated : 1;
		uint32_t size : 31;
	};

	Header* buff;
	Header* firstFreeHeader;

	Header* end;
public:
	DynamicAllocator() {
		buff = new Header[buffSize];
		memset(buff, 0, buffSize * sizeof(Header));

		end = buff + buffSize;
		firstFreeHeader = buff;

		firstFreeHeader->size = buffSize;
	}

	~DynamicAllocator() {
		delete[] buff;
	}

	uint32_t allocId(uint32_t size) {
		if (size == 0) return 0;
		size = ((size + sizeof(Header)-1) / sizeof(Header)) + 1;

		Header *cursor = firstFreeHeader;

		while (cursor < end) {
			if (cursor->isAllocated || cursor->size < size) {
				cursor += cursor->size;
				continue;
			}

			cursor->isAllocated = 1;
			uint32_t oldSize = cursor->size;
			cursor->size = size;

			Header*cursor2 = cursor + size;

			if (oldSize > size) {
				cursor2->isAllocated = 0;
				cursor2->size = oldSize - size;
			}

			if (firstFreeHeader == cursor) {
				while (cursor2->isAllocated == 1 && cursor2 < end) {
					cursor2 += cursor2->size;
				}
				firstFreeHeader = cursor2;
			}

			return cursor + 1 - buff;
		}
		return 0;
	}

	void freeId(uint32_t id) {
		Header* cursor = buff + id - 1;
		cursor->isAllocated = 0;
		Header* next = cursor + cursor->size;
		if (next < end && next->isAllocated == 0) {
			cursor->size += next->size;
		}
		if (firstFreeHeader > cursor) {
			firstFreeHeader = cursor;
		}
	}

	void* alloc(uint32_t size) {
		if (size == 0) return nullptr;
		return (void*)(buff + allocId(size));
	}

	void free(void* p) {
		freeId(((Header*)p) - buff);
	}

	template<class T>
	T* alloc(int count = 0) {
		uint32_t size = sizeof(T)* count;
		return alloc(size);
	}


	template<class T>
	uint32_t allocId(int count = 0) {
		uint32_t size = sizeof(T)* count;
		return allocId(size);
	}
};

#endif