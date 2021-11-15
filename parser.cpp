#include "parser.h"
#include "lox.h"

Parser::Parser(const std::vector<Token> &parserTokens) :
	tokens(parserTokens)
{
}

std::shared_ptr<Expr>
Parser::parse()
{
	try
	{
		return expression();
	}
	catch (const ParseError &e)
	{
		return std::shared_ptr<NilLiteral>();
	}
}

std::shared_ptr<Expr>
Parser::expression()
{
		return equality();
}

std::shared_ptr<Expr>
Parser::equality()
{
	std::shared_ptr<Expr> expr = comparison();

	const std::vector<TokenType> tokenTypes = {
		BANG_EQUAL,
		EQUAL_EQUAL
	};
	while (match(tokenTypes))
	{
		std::shared_ptr<Token> op = std::make_shared<Token>(previous());
		std::shared_ptr<Expr> right = comparison();
		expr = std::make_shared<Binary>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr>
Parser::comparison()
{
	std::shared_ptr<Expr> expr = term();

	const std::vector<TokenType> tokenTypes = {
		GREATER,
		GREATER_EQUAL,
		LESS,
		LESS_EQUAL
	};
	while (match(tokenTypes))
	{
		std::shared_ptr<Token> op = std::make_shared<Token>(previous());
		std::shared_ptr<Expr> right = term();
		expr = std::make_shared<Binary>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr>
Parser::term()
{
	std::shared_ptr<Expr> expr = factor();

	const std::vector<TokenType> tokenTypes = {
		MINUS,
		PLUS
	};
	while (match(tokenTypes))
	{
		std::shared_ptr<Token> op = std::make_shared<Token>(previous());
		std::shared_ptr<Expr> right = factor();
		expr = std::make_shared<Binary>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr>
Parser::factor()
{
	std::shared_ptr<Expr> expr = unary();

	const std::vector<TokenType> tokenTypes = {
		SLASH,
		STAR
	};
	while (match(tokenTypes))
	{
		std::shared_ptr<Token> op = std::make_shared<Token>(previous());
		std::shared_ptr<Expr> right = unary();
		expr = std::make_shared<Binary>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr>
Parser::unary()
{
	const std::vector<TokenType> tokenTypes = {
		BANG,
		MINUS
	};
	if (match(tokenTypes))
	{
		std::shared_ptr<Token> op = std::make_shared<Token>(previous());
		std::shared_ptr<Expr> right = unary();
		return std::make_shared<Unary>(op, right);
	}

	return primary();
}

std::shared_ptr<Expr>
Parser::primary()
{
	if (match(FALSE))
	{
		return std::make_shared<BooleanLiteral>(false);
	}
	if (match(TRUE))
	{
		return std::make_shared<BooleanLiteral>(true);
	}
	if (match(NIL))
	{
		return std::make_shared<NilLiteral>();
	}

	if (match(STRING))
	{
		return std::make_shared<StringLiteral>(previous().string_literal);
	}
	if (match(NUMBER))
	{
		return std::make_shared<DoubleLiteral>(previous().double_literal);
	}

	if (match(LEFT_PAREN))
	{
		std::shared_ptr<Expr> expr = expression();
		consume(RIGHT_PAREN, L"Expect ')' after expression.");
		return std::make_shared<Grouping>(expr);
	}

	throw error(peek(), L"Expect expression.");
}

Token
Parser::consume(TokenType tokenType, const std::wstring &message)
{
	if (check(tokenType))
	{
		return advance();
	}

	throw error(peek(), message);
}

ParseError
Parser::error(Token token, const std::wstring &message)
{
	Lox::error(token, message);
	return ParseError(message);
}

void
Parser::synchronize()
{
	advance();

	while (!isAtEnd())
	{
		if (previous().type == SEMICOLON)
		{
			return;
		}

		switch (peek().type)
		{
		case CLASS:
		case FUN:
		case VAR:
		case FOR:
		case IF:
		case WHILE:
		case PRINT:
		case RETURN:
			return;
		default:
			break;
		}

		advance();
	}
}

bool
Parser::match(const std::vector<TokenType> &tokenTypes)
{
	for (auto tokenType : tokenTypes)
	{
		if (check(tokenType))
		{
			advance();
			return true;
		}
	}
	return false;
}

bool
Parser::match(TokenType tokenType)
{
	const std::vector<TokenType> tokenTypes = {
		tokenType
	};
	return match(tokenTypes);
}

bool
Parser::check(TokenType tokenType)
{
	if (isAtEnd())
	{
		return false;
	}
	return (peek().type == tokenType);
}

Token
Parser::advance()
{
	if (!isAtEnd())
	{
		current++;
	}
	return previous();
}

bool
Parser::isAtEnd()
{
	return (peek().type == END_OF_FILE);
}

Token
Parser::peek()
{
	return tokens.at(current);
}

Token
Parser::previous()
{
	return tokens.at(current - 1);
}
