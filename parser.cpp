#include "parser.h"
#include "lox.h"

Parser::Parser(const std::vector<std::shared_ptr<Token>> &parserTokens) :
	tokens(parserTokens)
{
}

std::vector<std::shared_ptr<Stmt>>
Parser::parse()
{
	std::vector<std::shared_ptr<Stmt>> statements;
	try
	{
		while (!isAtEnd())
		{
			statements.push_back(declaration());
		}
	}
	catch (const ParseError &)
	{
	}
	return statements;
}

std::shared_ptr<Stmt>
Parser::declaration()
{
	try
	{
		if (match(FUN))
		{
			return functionX(L"function");
		}
		if (match(VAR))
		{
			return varDeclaration();
		}

		return statement();
	}
	catch (const ParseError &e)
	{
		synchronize();
		return std::shared_ptr<Stmt>();
	}
}

std::shared_ptr<Stmt>
Parser::functionX(const std::wstring &kind)
{
	std::shared_ptr<Token> name = consume(IDENTIFIER, L"Expect" + kind + L" name.");
	consume(LEFT_PAREN, L"Expect '(' after " + kind + L" name.");
	std::vector<std::shared_ptr<Token>> parameters;
	if (!check(RIGHT_PAREN))
	{
		do
		{
			if (parameters.size() >= 255)
			{
				error(peek(), L"Can't have more than 255 parameters.");
			}

			parameters.push_back(consume(IDENTIFIER, L"Expect parameter name."));
		}
		while (match(COMMA));
	}
	consume(RIGHT_PAREN, L"Expect ')' after parameters.");

	consume(LEFT_BRACE, L"Expect '{' before body.");
	std::vector<std::shared_ptr<Stmt>> body = block();
	return std::make_shared<Function>(name, parameters, body);
}

std::shared_ptr<Stmt>
Parser::varDeclaration()
{
	std::shared_ptr<Token> name = consume(IDENTIFIER, L"Expect variable name.");

	std::shared_ptr<Expr> initializer;
	if (match(EQUAL))
	{
		initializer = expression();
	}
	consume(SEMICOLON, L"Expect ';' after variable declaration.");
	return std::make_shared<Var>(name, initializer);
}

std::shared_ptr<Stmt>
Parser::statement()
{
	if (match(FOR))
	{
		return forStatement();
	}
	if (match(IF))
	{
		return ifStatement();
	}
	if (match(PRINT))
	{
		return printStatement();
	}
	if (match(WHILE))
	{
		return whileStatement();
	}
	if (match(LEFT_BRACE))
	{
		return std::make_shared<Block>(block());
	}
	return expressionStatement();
}

std::shared_ptr<Stmt>
Parser::forStatement()
{
	consume(LEFT_PAREN, L"Expect '(' after for.");
	std::shared_ptr<Stmt> initializer;
	if (match(SEMICOLON))
	{
		// No initializer
	}
	else if (match(VAR))
	{
		initializer = varDeclaration();
	}
	else
	{
		initializer = expressionStatement();
	}

	std::shared_ptr<Expr> condition;
	if (!check(SEMICOLON))
	{
		condition = expression();
	}
	else
	{
		condition = std::make_shared<BooleanLiteral>(true);
	}
	consume(SEMICOLON, L"Expect ';' after loop condition.");

	std::shared_ptr<Expr> increment;
	if (!check(RIGHT_PAREN))
	{
		increment = expression();
	}
	consume(RIGHT_PAREN, L"Expect ')' after for clauses.");

	std::shared_ptr<Stmt> body = statement();

	if (increment)
	{
		std::vector<std::shared_ptr<Stmt>> statements;
		statements.push_back(body);
		statements.push_back(std::make_shared<Expression>(increment));
		body = std::make_shared<Block>(statements);
	}

	body = std::make_shared<While>(condition, body);

	if (initializer)
	{
		std::vector<std::shared_ptr<Stmt>> statements;
		statements.push_back(initializer);
		statements.push_back(body);
		body = std::make_shared<Block>(statements);
	}

	return body;
}

std::shared_ptr<Stmt>
Parser::ifStatement()
{
	consume(LEFT_PAREN, L"Expect '(' after if.");
	std::shared_ptr<Expr> condition = expression();
	consume(RIGHT_PAREN, L"Expect ')' after if condition.");

	std::shared_ptr<Stmt> thenBranch = statement();
	std::shared_ptr<Stmt> elseBranch;
	if (match(ELSE))
	{
		elseBranch = statement();
	}
	return std::make_shared<If>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Stmt>
Parser::whileStatement()
{
	consume(LEFT_PAREN, L"Expect '(' after while.");
	std::shared_ptr<Expr> condition = expression();
	consume(RIGHT_PAREN, L"Expect ')' after if condition.");

	std::shared_ptr<Stmt> body = statement();
	return std::make_shared<While>(condition, body);
}

std::shared_ptr<Stmt>
Parser::printStatement()
{
	std::shared_ptr<Expr> value = expression();
	consume(SEMICOLON, L"Expect ';' after value.");
	return std::make_shared<Print>(value);
}

std::shared_ptr<Stmt>
Parser::expressionStatement()
{
	std::shared_ptr<Expr> value = expression();
	consume(SEMICOLON, L"Expect ';' after expression.");
	return std::make_shared<Expression>(value);
}

std::vector<std::shared_ptr<Stmt>>
Parser::block()
{
	std::vector<std::shared_ptr<Stmt>> statements;

	while (!check(RIGHT_BRACE) && !isAtEnd())
	{
		statements.push_back(declaration());
	}

	consume(RIGHT_BRACE, L"Expect '}' after block.");
	return statements;
}

std::shared_ptr<Expr>
Parser::expression()
{
	return assignment();
}

std::shared_ptr<Expr>
Parser::assignment()
{
	std::shared_ptr<Expr> expr = logicalOr();

	if (match(EQUAL))
	{
		std::shared_ptr<Token> equals = previous();
		std::shared_ptr<Expr> value = assignment();

		try
		{
			auto variable = std::dynamic_pointer_cast<Variable>(expr);
			std::shared_ptr<Token> name = variable->m_name;
			return std::make_shared<Assign>(name, value);
		}
		catch (const std::bad_any_cast &e)
		{
		}

		error(equals, L"Invalid assignment target.");
	}

	return expr;

}

std::shared_ptr<Expr>
Parser::logicalOr()
{
	std::shared_ptr<Expr> expr = logicalAnd();

	while (match(OR))
	{
		std::shared_ptr<Token> operatorX = previous();
		std::shared_ptr<Expr> right = logicalAnd();
		expr = std::make_shared<Logical>(expr, operatorX, right);
	}
	return expr;
}

std::shared_ptr<Expr>
Parser::logicalAnd()
{
	std::shared_ptr<Expr> expr = equality();

	while (match(AND))
	{
		std::shared_ptr<Token> operatorX = previous();
		std::shared_ptr<Expr> right = equality();
		expr = std::make_shared<Logical>(expr, operatorX, right);
	}
	return expr;
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
		std::shared_ptr<Token> op = previous();
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
		std::shared_ptr<Token> op = previous();
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
		std::shared_ptr<Token> op = previous();
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
		std::shared_ptr<Token> op = previous();
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
		std::shared_ptr<Token> op = previous();
		std::shared_ptr<Expr> right = unary();
		return std::make_shared<Unary>(op, right);
	}

	return call();
}

std::shared_ptr<Expr>
Parser::call()
{
	std::shared_ptr<Expr> expr = primary();

	while (true)
	{
		if (match(LEFT_PAREN))
		{
			expr = finishCall(expr);
		}
		else
		{
			break;
		}
	}
	return expr;
}

std::shared_ptr<Expr>
Parser::finishCall(std::shared_ptr<Expr> callee)
{
	std::vector<std::shared_ptr<Expr>> arguments;
	if (!check(RIGHT_PAREN))
	{
		do
		{
			if (arguments.size() >= 255)
			{
				error(peek(), L"Can't have more than 255 arguments.");
			}
			arguments.push_back(expression());
		}
		while (match(COMMA));
	}

	std::shared_ptr<Token> paren = consume(RIGHT_PAREN, L"Expect ')' after arguments.");
	return std::make_shared<Call>(callee, paren, arguments);
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
		return std::make_shared<StringLiteral>(previous()->string_literal);
	}
	if (match(NUMBER))
	{
		return std::make_shared<DoubleLiteral>(previous()->double_literal);
	}
	if (match(IDENTIFIER))
	{
		return std::make_shared<Variable>(previous());
	}

	if (match(LEFT_PAREN))
	{
		std::shared_ptr<Expr> expr = expression();
		consume(RIGHT_PAREN, L"Expect ')' after expression.");
		return std::make_shared<Grouping>(expr);
	}

	throw error(peek(), L"Expect expression.");
}

std::shared_ptr<Token>
Parser::consume(TokenType tokenType, const std::wstring &message)
{
	if (check(tokenType))
	{
		return advance();
	}

	throw error(peek(), message);
}

ParseError
Parser::error(std::shared_ptr<Token> token, const std::wstring &message)
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
		if (previous()->type == SEMICOLON)
		{
			return;
		}

		switch (peek()->type)
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
	return (peek()->type == tokenType);
}

std::shared_ptr<Token>
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
	return (peek()->type == END_OF_FILE);
}

std::shared_ptr<Token>
Parser::peek()
{
	return tokens.at(current);
}

std::shared_ptr<Token>
Parser::previous()
{
	return tokens.at(current - 1);
}
