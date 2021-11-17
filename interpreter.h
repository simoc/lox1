#pragma once

#include <string>
#include <memory>
#include <vector>

#include "token.h"
#include "expr.h"
#include "stmt.h"

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
	std::any visitBinaryExpr(std::shared_ptr<Binary> expr);

	std::any visitGroupingExpr(std::shared_ptr<Grouping> expr);

	std::any visitDoubleLiteralExpr(std::shared_ptr<DoubleLiteral> expr);

	std::any visitStringLiteralExpr(std::shared_ptr<StringLiteral> expr);

	std::any visitBooleanLiteralExpr(std::shared_ptr<BooleanLiteral> expr);

	std::any visitNilLiteralExpr(std::shared_ptr<NilLiteral> expr);

	std::any visitUnaryExpr(std::shared_ptr<Unary> expr);

	std::any visitPrintStmt(std::shared_ptr<Print> expr);

	std::any visitExpressionStmt(std::shared_ptr<Expression> expr);

	void interpret(std::vector<std::shared_ptr<Stmt>> statements);

private:
	std::any evaluate(std::shared_ptr<Expr> expr);

	std::wstring stringify(std::any n);

	bool isEqual(std::any a, std::any b);

	void checkNumberOperand(std::shared_ptr<Token> operatorX, std::any operand);

	void checkNumberOperands(std::shared_ptr<Token> operatorX, std::any left, std::any right);

	bool isTruthy(std::any n);

	void execute(std::shared_ptr<Stmt> stmt);
};
