#pragma once

#include <string>
#include <memory>
#include <vector>

#include "token.h"
#include "expr.h"
#include "stmt.h"
#include "environment.h"

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
	Interpreter();

	std::any visitAssignExpr(std::shared_ptr<Assign> expr);

	std::any visitBlockStmt(std::shared_ptr<Block> stmt);

	std::any visitBinaryExpr(std::shared_ptr<Binary> expr);

	std::any visitGroupingExpr(std::shared_ptr<Grouping> expr);

	std::any visitDoubleLiteralExpr(std::shared_ptr<DoubleLiteral> expr);

	std::any visitStringLiteralExpr(std::shared_ptr<StringLiteral> expr);

	std::any visitBooleanLiteralExpr(std::shared_ptr<BooleanLiteral> expr);

	std::any visitNilLiteralExpr(std::shared_ptr<NilLiteral> expr);

	std::any visitLogicalExpr(std::shared_ptr<Logical> expr);

	std::any visitUnaryExpr(std::shared_ptr<Unary> expr);

	std::any visitVariableExpr(std::shared_ptr<Variable> expr);

	std::any visitIfStmt(std::shared_ptr<If> stmt);

	std::any visitPrintStmt(std::shared_ptr<Print> expr);

	std::any visitExpressionStmt(std::shared_ptr<Expression> expr);

	std::any visitVarStmt(std::shared_ptr<Var> expr);

	std::any visitWhileStmt(std::shared_ptr<While> stmt);

	void interpret(std::vector<std::shared_ptr<Stmt>> statements);

private:
	std::shared_ptr<Environment> environment;

	std::shared_ptr<Expr> castToExpr(std::any value);

	void executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> env);

	std::any evaluate(std::shared_ptr<Expr> expr);

	std::wstring stringify(std::any n);

	bool isEqual(std::any a, std::any b);

	void checkNumberOperand(std::shared_ptr<Token> operatorX, std::any operand);

	void checkNumberOperands(std::shared_ptr<Token> operatorX, std::any left, std::any right);

	bool isTruthy(std::any n);

	void execute(std::shared_ptr<Stmt> stmt);
};
