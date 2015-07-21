#include "Tokenizer.h"

void Tokenizer::buildTokenList() {
	addToken(Tt_whitespace, [](const char *code, TokenList& tokenDataList) {
		const char *i = code;
		while (*i == ' ' || *i == '\n' || *i == '\t') i++;
		return i - code > 0 ? (int)(i - code) : INVALID;
	}).makeAnonymous();

	addToken(Tt_comment_single, [](const char *code, TokenList& tokenDataList) {
		const char *i = code;
		if (*i++ == '/' && *i++ == '/') {
			while (*i != '\0' && *i != '\n') i++;
			return (int)(i - code + (*i ? 1 : 0));
		}
		return INVALID;
	}).makeAnonymous();

	addToken(Tt_comment_multi, [](const char *code, TokenList& tokenDataList) {
		const char *i = code;
		if (*i++ == '/' && *i++ == '*') {
			while (*i != '\0' && (*i != '*' && *(i + 1) != '/')) i++;
			return *i ? (int)(i - code + 2) : INVALID;
		}
		return INVALID;
	}).makeAnonymous();


	addToken(Tt_ws, zeroOrMore(oneOf(Tt_whitespace, Tt_comment_single, Tt_comment_multi))).makeAnonymous();

	addToken(Tt_empty, fromString(""));

	addToken(Tt_integer, [this](const char *code, TokenList& tokenDataList) {
		const char *i = code;
		if (isDigit(*i++)) {
			while (isDigit(*i++));
			return (int)(i - code - 1);
		}
		return INVALID;
	});

	addToken(Tt_float, [this](const char *code, TokenList& tokenDataList) {
		const char *i = code;
		while (isDigit(*i)) i++;
		if (*i++ == '.') {
			if (isDigit(*i++)) {
				while (isDigit(*i++));
				return (int)(i - code - 1);
			}
		}
		return INVALID;
	});

	addToken(Tt_operator, oneOf(
		fromString("+"),
		fromString("-"),
		fromString("*"),
		fromString("/"),
		fromString("%"),
		fromString("<"),
		fromString(">"),
		fromString("<="),
		fromString(">="),
		fromString("!="),
		fromString("&&"),
		fromString("||"),
		fromString("&"),
		fromString("|"),
		fromString("<<"),
		fromString(">>"),
		fromString("^"),
		fromString("=="))
		);

	addToken(Tt_semicolon, fromString(";")).makeAnonymous();
	addToken(Tt_if, fromString("if")).makeAnonymous();
	addToken(Tt_p_left, fromString("(")).makeAnonymous();
	addToken(Tt_p_right, fromString(")")).makeAnonymous();
	addToken(Tt_cp_left, fromString("{")).makeAnonymous();
	addToken(Tt_cp_right, fromString("}")).makeAnonymous();
	addToken(Tt_for, fromString("for")).makeAnonymous();

	addToken(Tt_while, fromString("while")).makeAnonymous();
	addToken(Tt_do, fromString("do")).makeAnonymous();

	addToken(Tt_string, [this](const char *code, TokenList& tokenDataList) {
		const char *i = code;
		if (*i == '"') {
			i++;
			while (*i && *i != '"') {
				if (*i == '\\' && *(i + 1) != '\0') i++;
				i++;
			}
			if (*i) {
				return (int)(i - code + 1);
			}
		} else if (*i == '\'') {
			i++;
			while (*i && *i != '\'') {
				if (*i == '\\' && *(i + 1) != '\0') i++;
				i++;
			}
			if (*i) {
				return (int)(i - code + 1);
			}
		}
		return INVALID;
	});

	addToken(Tt_variable, [this](const char *code, TokenList& tokenDataList) {
		const char *i = code;
		if (isLetter(*i++)) {
			while (isDigit(*i) || isLetter(*i)) i++;
			return (int)(i - code);
		}
		return INVALID;
	});

	addToken(Tt_singleoperator,
		oneOf(
		fromString("++"),
		fromString("--")
		)
		);

	addToken(Tt_variable_withpost,
		join(Tt_variable, Tt_singleoperator)
		);

	addToken(Tt_variable_withpre,
		join(Tt_singleoperator, Tt_variable)
		);

	addToken(Tt_unary, oneOf(fromString("-"), fromString("+"), fromString("!")));

	addToken(Tt_value_one,
		oneOf(Tt_assignment, Tt_funccall, Tt_variable, Tt_integer, Tt_float, Tt_string, Tt_variable_withpre,
		Tt_variable_withpost, join(Tt_unary, Tt_value_one), join(Tt_p_left, Tt_value, Tt_p_right),
		Tt_funcdef
		)
		).makeAnonymous();

	addToken(Tt_value, oneOf(
		join(Tt_value_one, zeroOrMore(join(Tt_operator, Tt_value_one))),
		join(Tt_value_one, zeroOrMore(join(Tt_operator, Tt_value)))
		)
		);

	addToken(Tt_assign, fromString("=")).makeAnonymous();

	addToken(Tt_assignment, join(Tt_variable, Tt_assign, Tt_value));


	addToken(Tt_multiple_statement, oneOrMore(Tt_statement));

	addToken(Tt_return, join(fromString("return"), atMostOne(Tt_value)));
	addToken(Tt_break, fromString("break"));

	addToken(Tt_statement,
		oneOf(
		join(Tt_break, Tt_semicolon),
		join(Tt_return, Tt_semicolon),
		join(Tt_value, Tt_semicolon),
		join(Tt_cp_left, Tt_multiple_statement, Tt_cp_right),
		Tt_ifcond,
		Tt_forloop,
		Tt_whileloop,
		Tt_dowhileloop,
		Tt_semicolon
		)
		);

	addToken(Tt_funcdef, join(
		fromString("func"),
		Tt_p_left,
		atMostOne(join(Tt_variable, zeroOrMore(join(fromString(","), Tt_variable)))),
		Tt_p_right,
		Tt_cp_left, Tt_statement, Tt_cp_right)
		);

	addToken(Tt_funccall,
		join(Tt_variable,
		Tt_p_left,
		atMostOne(join(Tt_value, zeroOrMore(join(fromString(","), Tt_value)))),
		Tt_p_right)
		);

	addToken(Tt_else, fromString("else"));

	addToken(Tt_ifcond,
		join(
		join(Tt_if, Tt_p_left, Tt_value, Tt_p_right, Tt_statement),
		zeroOrMore(join(Tt_else, Tt_statement))
		)
		);

	addToken(Tt_forloop,
		join(
		Tt_for,
		Tt_p_left,
		Tt_value, Tt_semicolon,
		Tt_value, Tt_semicolon,
		Tt_value,
		Tt_p_right,
		Tt_statement
		)
		);

	addToken(Tt_whileloop,
		join(
		Tt_while,
		Tt_p_left, Tt_value, Tt_p_right,
		Tt_statement
		)
		);


	addToken(Tt_dowhileloop, join(Tt_do, Tt_statement,
		Tt_while, Tt_p_left, Tt_value, Tt_p_right, Tt_semicolon));

	addToken(Tt_program, oneOrMore(Tt_statement));
}


bool Tokenizer::tokenize(TokenType tokenType, const char* code, std::vector<Token>& tokenTree) {
	int l = getToken(tokenType).validate(code, tokenTree);
	return l >= 0;
}