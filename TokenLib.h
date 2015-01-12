#ifndef TOKENLIB_H
#define TOKENLIB_H

typedef enum {
	ERROR,
	PROGRAM,
	INTEGER,
	FLOAT,
	STRING,
	OPERATOR,
	VARIABLE,
	VALUE,
	ASSIGNMENT,
	STATEMENT,
	IFCOND,
	ELSE,
	WHILE,
	BREAK,
	FOR,
	MAXTOKEN
} tokenType;


typedef struct TokenStruct{
	tokenType type;
	struct TokenStruct *nextSibling;
	struct TokenStruct *firstChild;
	
	int line;

	void *extra;
} Token;

int isSingle(tokenType t);


Token* createToken(char *c);


int buildProgram(Token **place);

void print(Token *t, int tab=0, int line=1);

#endif
