#include "Tools.h"

const char* operatorStrings[] = {
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

	"++",
	"--",
};


char* tokenNames[Tt_count];

void buildTools() {
	tokenNames[Tt_whitespace] = "whitespace";
	tokenNames[Tt_comment_single] = "comment_single";
	tokenNames[Tt_comment_multi] = "comment_multi";
	tokenNames[Tt_ws] = "ws";
	tokenNames[Tt_empty] = "empty";
	tokenNames[Tt_integer] = "integer";
	tokenNames[Tt_float] = "float";
	tokenNames[Tt_operator] = "operator";
	tokenNames[Tt_semicolon] = "semicolon";
	tokenNames[Tt_if] = "if";
	tokenNames[Tt_p_left] = "p_left";
	tokenNames[Tt_p_right] = "p_right";
	tokenNames[Tt_cp_left] = "cp_left";
	tokenNames[Tt_cp_right] = "cp_right";
	tokenNames[Tt_for] = "for";
	tokenNames[Tt_while] = "while";
	tokenNames[Tt_do] = "do";
	tokenNames[Tt_string] = "string";
	tokenNames[Tt_variable] = "variable";
	tokenNames[Tt_singleoperator] = "singleoperator";
	tokenNames[Tt_variable_withpost] = "variable_withpost";
	tokenNames[Tt_variable_withpre] = "variable_withpre";
	tokenNames[Tt_unary] = "unary";
	tokenNames[Tt_value_with_unary] = "value_with_unary";
	tokenNames[Tt_value] = "value";
	tokenNames[Tt_value_one] = "value_one";
	tokenNames[Tt_assignment] = "assignment";
	tokenNames[Tt_funccall] = "funccall";
	tokenNames[Tt_statement] = "statement";
	tokenNames[Tt_multiple_statement] = "multiple_statement";
	tokenNames[Tt_return] = "return";
	tokenNames[Tt_break] = "break";
	tokenNames[Tt_ifcond] = "ifcond";
	tokenNames[Tt_forloop] = "forloop";
	tokenNames[Tt_whileloop] = "whileloop";
	tokenNames[Tt_dowhileloop] = "dowhileloop";
	tokenNames[Tt_funcdef] = "funcdef";
	tokenNames[Tt_else] = "else";
	tokenNames[Tt_program] = "program";
	tokenNames[Tt_assign] = "assign";

	tokenNames[Tt_invalid] = "invalid";
}

const char* getTokenName(TokenType type) {
	return tokenNames[type];
}

const char* getOperatorString(int type) {
	return operatorStrings[type];
}
