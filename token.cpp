#include "token.h"

Token::Token(TokenType _type, const std::wstring &_lexeme, int _line) :
	type(_type),
	lexeme(_lexeme),
	line(_line)
{
}

Token::Token(TokenType _type, const std::wstring &_lexeme, const std::wstring &_literal, int _line) :
	type(_type),
	lexeme(_lexeme),
	string_literal(_literal),
	line(_line)
{
}

Token::Token(TokenType _type, const std::wstring &_lexeme, double _literal, int _line) :
	type(_type),
	lexeme(_lexeme),
	double_literal(_literal),
	line(_line)
{
}

std::wostream& operator<< (std::wostream& stream, const Token& token)
{
	stream << token.type << " " << token.lexeme << " ";
       	if (token.type == NUMBER)
		stream << token.double_literal;
	else
		stream << token.string_literal;
	return stream;
}
