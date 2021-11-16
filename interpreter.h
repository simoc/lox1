#pragma once

#include <string>
#include <memory>

#include "token.h"
#include "expr.h"

class Interpreter : public Visitor
{
public:
	std::any visitBinaryExpr(std::shared_ptr<Binary> expr);

	std::any visitGroupingExpr(std::shared_ptr<Grouping> expr);

	std::any visitDoubleLiteralExpr(std::shared_ptr<DoubleLiteral> expr);

	std::any visitStringLiteralExpr(std::shared_ptr<StringLiteral> expr);

	std::any visitBooleanLiteralExpr(std::shared_ptr<BooleanLiteral> expr);

	std::any visitNilLiteralExpr(std::shared_ptr<NilLiteral> expr);

	std::any visitUnaryExpr(std::shared_ptr<Unary> expr);

	void interpret(std::shared_ptr<Expr> expr);

private:
	std::any evaluate(std::shared_ptr<Expr> expr);

	std::wstring stringify(std::any n);

	bool isEqual(std::any a, std::any b);

	void checkNumberOperand(std::shared_ptr<Token> operatorX, std::any operand);

	void checkNumberOperands(std::shared_ptr<Token> operatorX, std::any left, std::any right);

	bool isTruthy(std::any n);
};
