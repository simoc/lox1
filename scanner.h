#pragma once

#include <vector>
#include <string>
#include <map>

#include "token.h"

class Scanner
{
public:
	Scanner(const std::wstring &bytes);

	std::vector<Token> scanTokens();
private:
	bool isAtEnd() const;
	void scanToken();
	void identifier();
	bool isAlpha(wchar_t c);
	bool isAlphaNumeric(wchar_t c);
	bool isDigit(wchar_t c);
	void number();
	wchar_t peekNext();
	void string();
	wchar_t peek();
	bool match(wchar_t expected);
	wchar_t advance();
	void addToken(TokenType type);
	void addToken(TokenType type, double literal);
	void addToken(TokenType type, const std::wstring &literal);

	std::wstring source;
	std::vector<Token> tokens;
	int start = 0;
	int current = 0;
	int line = 1;
	static const std::map<std::wstring, TokenType> keywords;
};
