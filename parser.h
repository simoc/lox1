#pragma once

#include <string>
#include <vector>
#include <memory>
#include "token.h"
#include "tokentype.h"
#include "expr.h"
#include "parseerror.h"
#include "lox.h"

class Parser
{
public:
	Parser(const std::vector<Token> &parserTokens) :
		tokens(parserTokens)
	{
	}

	std::shared_ptr<Expr> parse()
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

	std::shared_ptr<Expr> expression()
	{
		return equality();
	}

	std::shared_ptr<Expr> equality()
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

	std::shared_ptr<Expr> comparison()
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

	std::shared_ptr<Expr> term()
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

	std::shared_ptr<Expr> factor()
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

	std::shared_ptr<Expr> unary()
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

	std::shared_ptr<Expr> primary()
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

		const std::vector<TokenType> tokenTypes = {
			NUMBER,
			STRING
		};
		if (match(tokenTypes))
		{
			return std::make_shared<StringLiteral>(previous().string_literal);
		}

		if (match(LEFT_PAREN))
		{
			std::shared_ptr<Expr> expr = expression();
			consume(RIGHT_PAREN, L"Expect ')' after expression.");
			return std::make_shared<Grouping>(expr);
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
			default:
				break;
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
