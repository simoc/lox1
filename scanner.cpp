#include "scanner.h"
#include "lox.h"

const std::map<std::wstring, TokenType> Scanner::keywords =
{
	{L"and", AND},
	{L"class", CLASS},
	{L"else", ELSE},
	{L"false", FALSE},
	{L"for", FOR},
	{L"fun", FUN},
	{L"if", IF},
	{L"nil", NIL},
	{L"or", OR},
	{L"print", PRINT},
	{L"return", RETURN},
	{L"super", SUPER},
	{L"this", THIS},
	{L"true", TRUE},
	{L"var", VAR},
	{L"while", WHILE}
};

Scanner::Scanner(const std::wstring &bytes) :
	source(bytes)
{
}

std::vector<Token>
Scanner::scanTokens()
{
	while (!isAtEnd())
	{
		start = current;
		scanToken();
	}
	tokens.push_back(Token(END_OF_FILE, std::wstring(), nullptr, line));
	return tokens;
}

bool
Scanner::isAtEnd() const
{
	return current >= source.length();
}

void
Scanner::scanToken()
{
	wchar_t c = advance();
	switch (c)
	{
		case '(':
			addToken(LEFT_PAREN);
			break;
		case ')':
			addToken(RIGHT_PAREN);
			break;
		case '{':
			addToken(LEFT_BRACE);
			break;
		case '}':
			addToken(RIGHT_BRACE);
			break;
		case ',':
			addToken(COMMA);
			break;
		case '.':
			addToken(DOT);
			break;
		case '-':
			addToken(MINUS);
			break;
		case ';':
			addToken(SEMICOLON);
			break;
		case '*':
			addToken(STAR);
			break;
		case '!':
			addToken(match('=') ? BANG_EQUAL : BANG);
			break;
		case '=':
			addToken(match('=') ? EQUAL_EQUAL : EQUAL);
			break;
		case '<':
			addToken(match('=') ? LESS_EQUAL : LESS);
			break;
		case '>':
			addToken(match('=') ? GREATER_EQUAL : GREATER);
			break;
		case '/':
			if (match('/'))
			{
				// A comment goes until the end of the line.
				while (peek() != '\n' && !isAtEnd())
				{
					advance();
				}
			}
			else
			{
				addToken(SLASH);
			}
			break;
		case ' ':
		case '\r':
		case '\t':
			// Ignore whitespace.
			break;
		case '\n':
			line++;
			break;
		case '"':
			string();
			break;
		default:
			if (isDigit(c))
			{
				number();
			}
			else if (isAlpha(c))
			{
				identifier();
			}
			else
			{
				Lox::error(line, L"Unexpected character.");
			}
			break;
	}
}

void
Scanner::identifier()
{
	while (isAlphaNumeric(peek()))
	{
		advance();
	}
	std::wstring text = source.substr(start, current - start);
	auto it = keywords.find(text);
	if (it == keywords.end())
		addToken(IDENTIFIER);
	else
		addToken(it->second);
}

bool
Scanner::isAlpha(wchar_t c)
{
	return ((c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_');
}

bool
Scanner::isAlphaNumeric(wchar_t c)
{
	return isAlpha(c) || isDigit(c);
}

bool
Scanner::isDigit(wchar_t c)
{
	return (c >= '0' && c <= '9');
}

void
Scanner::number()
{
	while (isDigit(peek()))
	{
		advance();
	}

	// Look for the fractional part.
	if (peek() == '.' && isDigit(peekNext()))
	{
		// Consume the "."
		advance();

		while (isDigit(peek()))
		{
			advance();
		}
	}

	auto count = current - start;
	addToken(NUMBER, std::stod(source.substr(start, count)));
}

wchar_t
Scanner::peekNext()
{
	if (current + 1 > source.length())
	{
		return '\0';
	}
	return source.at(current + 1);
}

void
Scanner::string()
{
	while (peek() != '"' && !isAtEnd())
	{
		if (peek() == '\n')
		{
			line++;
		}
		advance();
	}

	if (isAtEnd())
	{
		Lox::error(line, L"Unterminated string.");
	}

	// The closing ".
	advance();

	// Trim the surrounding quotes.
	auto count = current - start;
	std::wstring value = source.substr(start + 1, count - 2);
	addToken(STRING, value);
}

wchar_t
Scanner::peek()
{
	if (isAtEnd())
		return '\0';

	return source.at(current);
}

bool
Scanner::match(wchar_t expected)
{
	if (isAtEnd())
		return false;

	if (source.at(current) != expected)
		return false;

	current++;
	return true;
}

wchar_t
Scanner::advance()
{
	wchar_t c = source.at(current);
	current++;
	return c;
}

void
Scanner::addToken(TokenType type)
{
	std::wstring text = source.substr(start, current - start);
	tokens.push_back(Token(type, text, line));
}

void
Scanner::addToken(TokenType type, double literal)
{
	std::wstring text = source.substr(start, current- start);
	tokens.push_back(Token(type, text, literal, line));
}

void
Scanner::addToken(TokenType type, const std::wstring &literal)
{
	std::wstring text = source.substr(start, current - start);
	tokens.push_back(Token(type, text, literal, line));
}

