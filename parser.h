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
	Parser(const std::vector<std::shared_ptr<Token>> &parserTokens);

	std::vector<std::shared_ptr<Stmt>> parse();

	std::shared_ptr<Expr> expression();

	std::shared_ptr<Expr> assignment();

	std::shared_ptr<Expr> logicalOr();

	std::shared_ptr<Expr> logicalAnd();

	std::shared_ptr<Expr> equality();

	std::shared_ptr<Expr> comparison();

	std::shared_ptr<Expr> term();

	std::shared_ptr<Expr> factor();

	std::shared_ptr<Expr> unary();

	std::shared_ptr<Expr> primary();

	std::shared_ptr<Token> consume(TokenType tokenType, const std::wstring &message);

	ParseError error(std::shared_ptr<Token> token, const std::wstring &message);

	void synchronize();

private:
	std::vector<std::shared_ptr<Token>> tokens;
	std::wstring::size_type current = 0;

	std::shared_ptr<Stmt> declaration();

	std::shared_ptr<Stmt> varDeclaration();

	std::shared_ptr<Stmt> statement();

	std::shared_ptr<Stmt> forStatement();

	std::shared_ptr<Stmt> ifStatement();

	std::shared_ptr<Stmt> whileStatement();

	std::shared_ptr<Stmt> printStatement();

	std::shared_ptr<Stmt> expressionStatement();

	std::vector<std::shared_ptr<Stmt>> block();

	bool match(const std::vector<TokenType> &tokenTypes);

	bool match(TokenType tokenType);

	bool check(TokenType tokenType);

	std::shared_ptr<Token> advance();

	bool isAtEnd();

	std::shared_ptr<Token> peek();

	std::shared_ptr<Token> previous();
};
