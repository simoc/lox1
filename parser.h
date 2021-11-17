#pragma once

#include <string>
#include <vector>
#include <memory>
#include "token.h"
#include "tokentype.h"
#include "expr.h"
#include "parseerror.h"
#include "stmt.h"

class Parser
{
public:
	Parser(const std::vector<Token> &parserTokens);

	std::vector<std::shared_ptr<Stmt>> parse();

	std::shared_ptr<Expr> expression();

	std::shared_ptr<Expr> equality();

	std::shared_ptr<Expr> comparison();

	std::shared_ptr<Expr> term();

	std::shared_ptr<Expr> factor();

	std::shared_ptr<Expr> unary();

	std::shared_ptr<Expr> primary();

	Token consume(TokenType tokenType, const std::wstring &message);

	ParseError error(Token token, const std::wstring &message);

	void synchronize();

private:
	std::vector<Token> tokens;
	std::wstring::size_type current = 0;

	std::shared_ptr<Stmt> statement();

	std::shared_ptr<Stmt> printStatement();

	std::shared_ptr<Stmt> expressionStatement();

	bool match(const std::vector<TokenType> &tokenTypes);

	bool match(TokenType tokenType);

	bool check(TokenType tokenType);

	Token advance();

	bool isAtEnd();

	Token peek();

	Token previous();
};
