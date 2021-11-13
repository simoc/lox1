#pragma once

#include <string>
#include <vector>
#include <memory>
#include "token.h"
#include "tokentype.h"
#include "expr.h"
#include "parseerror.h"
#include "lox.h"

template <class R>
class Parser
{
public:
	Parser(const std::vector<Token> &parserTokens) :
		tokens(parserTokens)
	{
	}

	std::shared_ptr<Expr<R>> parse()
	{
		try
		{
			return expression();
		}
		catch (const ParseError &e)
		{
			return std::shared_ptr<NilLiteral<R>>();
		}
	}

	std::shared_ptr<Expr<R>> expression()
	{
		return equality();
	}

	std::shared_ptr<Expr<R>> equality()
	{
		std::shared_ptr<Expr<R>> expr = comparison();

		const std::vector<TokenType> tokenTypes = {
			BANG_EQUAL,
			EQUAL_EQUAL
		};
		while (match(tokenTypes))
		{
			std::shared_ptr<Token> op = std::make_shared<Token>(previous());
			std::shared_ptr<Expr<R>> right = comparison();
			expr = std::make_shared<Binary<R>>(expr, op, right);
		}
		return expr;
	}

	std::shared_ptr<Expr<R>> comparison()
	{
		std::shared_ptr<Expr<R>> expr = term();

		const std::vector<TokenType> tokenTypes = {
			GREATER,
			GREATER_EQUAL,
			LESS,
			LESS_EQUAL
		};
		while (match(tokenTypes))
		{
			std::shared_ptr<Token> op = std::make_shared<Token>(previous());
			std::shared_ptr<Expr<R>> right = term();
			expr = std::make_shared<Binary<R>>(expr, op, right);
		}
		return expr;
	}

	std::shared_ptr<Expr<R>> term()
	{
		std::shared_ptr<Expr<R>> expr = factor();

		const std::vector<TokenType> tokenTypes = {
			MINUS,
			PLUS
		};
		while (match(tokenTypes))
		{
			std::shared_ptr<Token> op = std::make_shared<Token>(previous());
			std::shared_ptr<Expr<R>> right = factor();
			expr = std::make_shared<Binary<R>>(expr, op, right);
		}
		return expr;
	}

	std::shared_ptr<Expr<R>> factor()
	{
		std::shared_ptr<Expr<R>> expr = unary();

		const std::vector<TokenType> tokenTypes = {
			SLASH,
			STAR
		};
		while (match(tokenTypes))
		{
			std::shared_ptr<Token> op = std::make_shared<Token>(previous());
			std::shared_ptr<Expr<R>> right = unary();
			expr = std::make_shared<Binary<R>>(expr, op, right);
		}
		return expr;
	}

	std::shared_ptr<Expr<R>> unary()
	{
		const std::vector<TokenType> tokenTypes = {
			BANG,
			MINUS
		};
		if (match(tokenTypes))
		{
			std::shared_ptr<Token> op = std::make_shared<Token>(previous());
			std::shared_ptr<Expr<R>> right = unary();
			return std::make_shared<Unary<R>>(op, right);
		}

		return primary();
	}

	std::shared_ptr<Expr<R>> primary()
	{
		if (match(FALSE))
		{
			return std::make_shared<BooleanLiteral<R>>(false);
		}
		if (match(TRUE))
		{
			return std::make_shared<BooleanLiteral<R>>(true);
		}
		if (match(NIL))
		{
			return std::make_shared<NilLiteral<R>>();
		}

		const std::vector<TokenType> tokenTypes = {
			NUMBER,
			STRING
		};
		if (match(tokenTypes))
		{
			return std::make_shared<StringLiteral<R>>(previous().string_literal);
		}

		if (match(LEFT_PAREN))
		{
			std::shared_ptr<Expr<R>> expr = expression();
			consume(RIGHT_PAREN, L"Expect ')' after expression.");
			return std::make_shared<Grouping<R>>(expr);
		}

		throw error(peek(), L"Expect expression.");
	}

	Token consume(TokenType tokenType, const std::wstring &message)
	{
		if (check(tokenType))
		{
			return advance();
		}

		throw error(peek(), message);
	}

	ParseError error(Token token, const std::wstring &message)
	{
		Lox::error(token, message);
		return ParseError(message);
	}

	void synchronize()
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
			}

			advance();
		}
	}

private:
	std::vector<Token> tokens;
	std::wstring::size_type current = 0;

	bool match(const std::vector<TokenType> &tokenTypes)
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

	bool match(TokenType tokenType)
	{
		const std::vector<TokenType> tokenTypes = {
			tokenType
		};
		return match(tokenTypes);
	}

	bool check(TokenType tokenType)
	{
		if (isAtEnd())
		{
			return false;
		}
		return (peek().type == tokenType);
	}

	Token advance()
	{
		if (!isAtEnd())
		{
			current++;
		}
		return previous();
	}

	bool isAtEnd()
	{
		return (peek().type == END_OF_FILE);
	}

	Token peek()
	{
		return tokens.at(current);
	}

	Token previous()
	{
		return tokens.at(current - 1);
	}
};
