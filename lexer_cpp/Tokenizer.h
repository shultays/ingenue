#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Tools.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>


class Tokenizer {
	static const int INVALID = -1;

	typedef std::function<int(const char*, TokenList&)> TokenValidator;

	class TokenDef : public TokenValidator {
	public:
		TokenType tokenType;
		bool valid;
		bool anonymous;

		TokenDef() {
			tokenType = Tt_invalid;
			valid = false;
			anonymous = false;
		}

		TokenDef(TokenValidator validator) : TokenValidator(validator) {
			tokenType = Tt_invalid;
			valid = true;
			anonymous = false;
		}

		TokenDef(TokenType tokenType, TokenValidator validator) : TokenValidator(validator) {
			this->tokenType = tokenType;
			valid = true;
			anonymous = false;
		}

		int validate(const char* code, TokenList& tokenDataList) const {
			Token data(tokenType, code);
			data.tokenLength = (*((TokenValidator*)this))(code, data.children);
			if (data.tokenLength >= 0) {
				if (anonymous == false) {
					mergeToParent(tokenDataList, data);
				} else {
					mergeToParent(tokenDataList, data.children);
				}
			}
			return data.tokenLength;
		}

		TokenDef& makeAnonymous() {
			anonymous = true;
			return *this;
		}
	};

	std::vector<TokenDef> tokenDefList;


	TokenDef& getToken(TokenType tokenType) {
		for (unsigned i = 0; i < tokenDefList.size(); i++) {
			if (tokenDefList[i].tokenType == tokenType) return tokenDefList[i];
		}
		TokenDef token;
		token.tokenType = tokenType;
		tokenDefList.push_back(token);
		return tokenDefList[tokenDefList.size() - 1];
	}

	class TokenDefList : public std::vector<TokenDef> {
		Tokenizer *tokenizer;
	public:
		TokenDefList(Tokenizer *tokenizer) {
			this->tokenizer = tokenizer;
		}
		void insert(TokenType tokenType) {
			push_back(tokenizer->getToken(tokenType));
		}
		void insert(const TokenDef& token) {
			push_back(token);
		}
	};

	TokenDef& addToken(TokenType tokenType, TokenValidator validator) {
		TokenDef& token = getToken(tokenType);
		token = TokenDef(tokenType, validator);
		return token;
	}

	template<typename T>
	void buildList(TokenDefList& list, T token) {
		list.insert(token);
	}

	template<typename T, typename... Args>
	void buildList(TokenDefList& list, T token, Args... args) {
		list.insert(token);
		buildList(list, args...);
	}
	// helpers

	TokenDef atMostOne(TokenDef tokenDef) {
		return TokenDef([this, tokenDef](const char *code, TokenList& tokenDataList) mutable {
			if (tokenDef.valid == false) tokenDef = getToken(tokenDef.tokenType);
			int l = tokenDef.validate(code, tokenDataList);
			return l == INVALID ? 0 : l;
		});
	}

	TokenDef atMostOne(TokenType tokenType) {
		return atMostOne(getToken(tokenType));
	}

	TokenDef zeroOrMore(TokenDef tokenDef) {
		return TokenDef([this, tokenDef](const char *code, TokenList& tokenDataList) mutable {
			if (tokenDef.valid == false) tokenDef = getToken(tokenDef.tokenType);
			const char *i = code;
			while (1) {
				int l = tokenDef.validate(i, tokenDataList);
				if (l == INVALID) break;
				i += l;
			}
			return (int)(i - code);
		});
	}

	TokenDef zeroOrMore(TokenType tokenType) {
		return zeroOrMore(getToken(tokenType));
	}

	TokenDef oneOrMore(TokenDef tokenDef) {
		return TokenDef([this, tokenDef](const char *code, TokenList& tokenDataList) mutable {
			if (tokenDef.valid == false) tokenDef = getToken(tokenDef.tokenType);
			const char *i = code;
			while (1) {
				int l = tokenDef.validate(i, tokenDataList);
				if (l == INVALID) break;
				i += l;
			}
			return i - code > 0 ? (int)(i - code) : INVALID;
		});
	}


	TokenDef oneOrMore(TokenType tokenType) {
		return oneOrMore(getToken(tokenType));
	}

	TokenDef fromString(const char* str) {
		return TokenDef([this, str](const char *code, TokenList& tokenDataList) {
			const char *i = code;
			const char *s = str;
			while (*s && *i && *s == *i) s++, i++;

			if (*s == '\0') {
				return (int)(i - code);
			}
			return INVALID;
		});
	}

	static void mergeToParent(TokenList& parentList, const Token& data) {
		if (data.tokenType != Tt_invalid) {
			parentList.push_back(data);
		} else {
			mergeToParent(parentList, data.children);
		}
	}

	static void mergeToParent(TokenList& parentList, const TokenList& child) {
		for (auto t = child.begin(); t != child.end(); t++) {
			if (t->tokenType != Tt_invalid) {
				parentList.push_back(*t);
			} else {
				mergeToParent(parentList, t->children);
			}
		}
	}

	template<typename... Args>
	TokenDef join(Args... args) {
		TokenDefList list(this);
		buildList(list, args...);
		return TokenDef([this, list](const char *code, TokenList& tokenDataList) mutable {
			const char *i = code;
			TokenDef ws = getToken(Tt_ws);
			i += ws.validate(i, tokenDataList);
			TokenList childList;
			for (auto t = list.begin(); t != list.end(); t++) {
				if (t->valid == false) *t = getToken(t->tokenType);
				int l = t->validate(i, childList);
				if (l == INVALID) return INVALID;
				i += l;

				i += ws.validate(i, childList);
			}
			mergeToParent(tokenDataList, childList);
			return (int)(i - code);
		});
	}

	template<typename... Args>
	TokenDef oneOf(Args... args) {
		TokenDefList list(this);
		buildList(list, args...);

		return TokenDef([this, list](const char *code, TokenList& tokenDataList) mutable {
			TokenList longestList;
			int m = INVALID;
			for (auto t = list.begin(); t != list.end(); t++) {
				if (t->valid == false) *t = getToken(t->tokenType);
				TokenList childList;
				int len = t->validate(code, childList);
				if (m < len) {
					m = len;
					longestList = childList;
				}
			}
			if (m >= 0) {
				mergeToParent(tokenDataList, longestList);
			}
			return m;
		});
	}

	static bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}
	static bool isLetter(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}
	void buildTokenList();

public:
	Tokenizer() {
		buildTokenList();
	}

	bool checkToken(const char* code) {
		return checkToken(Tt_program, code);
	}
	bool checkToken(TokenType tokenType, const char* code) {
		TokenList tokenDataList;
		return getToken(tokenType).validate(code, tokenDataList) >= 0;
	}

	bool tokenize(const char* code, TokenList& tokenDataList) {
		return tokenize(Tt_program, code, tokenDataList);
	}

	bool tokenize(TokenType tokenType, const char* code, TokenList& tokenDataList);

	void printList(const TokenList& list, int level = 0) {
		for (unsigned i = 0; i < list.size(); i++) {
			for (int j = 0; j < level * 2; j++) printf(" ");
			printf("%s (", getTokenName(list[i].tokenType));
			if (list[i].tokenLength < 20) {
				bool oldSpace = true;
				for (int k = 0; k < list[i].tokenLength; k++) {
					char c = list[i].tokenBegin[k];
					if (c == ' ' || c == '\n' || c == '\t') {
						if (oldSpace == false) {
							printf(" ");
							oldSpace = true;
						}
					} else {
						oldSpace = false;
						printf("%c", c);
					}
				}
			} else {

				printf("...");
			}
			printf(")\n");
			printList(list[i].children, level + 1);
		}
	}
};

#endif