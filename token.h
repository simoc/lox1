#pragma once

#include <iostream>

#include "tokentype.h"

class Token
{
public:
	TokenType type = END_OF_FILE;
	std::wstring lexeme;

	std::wstring string_literal;
	double double_literal = 0;
	int line = 0;

	Token(TokenType _type, const std::wstring &_lexeme, int _line);

	Token(TokenType _type, const std::wstring &_lexeme, const std::wstring &_literal, int _line);

	Token(TokenType _type, const std::wstring &_lexeme, double _literal, int _line);

	friend std::wostream& operator<< (std::wostream& out, const Token &token);
};
