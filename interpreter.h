#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "token.h"
#include "expr.h"
#include "stmt.h"
#include "environment.h"

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
	std::shared_ptr<Environment> globals;

	Interpreter();

	std::any visitAssignExpr(std::shared_ptr<Assign> expr);

	std::any visitBlockStmt(std::shared_ptr<Block> stmt);

	std::any visitClassStmt(std::shared_ptr<Class> stmt);

	std::any visitBinaryExpr(std::shared_ptr<Binary> expr);

	std::any visitCallExpr(std::shared_ptr<Call> expr);

	std::any visitGroupingExpr(std::shared_ptr<Grouping> expr);

	std::any visitDoubleLiteralExpr(std::shared_ptr<DoubleLiteral> expr);

	std::any visitStringLiteralExpr(std::shared_ptr<StringLiteral> expr);

	std::any visitBooleanLiteralExpr(std::shared_ptr<BooleanLiteral> expr);

	std::any visitNilLiteralExpr(std::shared_ptr<NilLiteral> expr);

	std::any visitLogicalExpr(std::shared_ptr<Logical> expr);

	std::any visitUnaryExpr(std::shared_ptr<Unary> expr);

	std::any visitVariableExpr(std::shared_ptr<Variable> expr);

	std::any visitIfStmt(std::shared_ptr<If> stmt);

	std::any visitFunctionStmt(std::shared_ptr<Function> stmt);

	std::any visitPrintStmt(std::shared_ptr<Print> expr);

	std::any visitReturnStmt(std::shared_ptr<Return> stmt);

	std::any visitExpressionStmt(std::shared_ptr<Expression> expr);

	std::any visitVarStmt(std::shared_ptr<Var> expr);

	std::any visitWhileStmt(std::shared_ptr<While> stmt);

	void interpret(std::vector<std::shared_ptr<Stmt>> statements);

	void executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> env);

	void resolve(std::shared_ptr<Expr> expr, int depth);

private:
	std::shared_ptr<Environment> environment;
	std::map<std::shared_ptr<Expr>, int> locals;

	std::shared_ptr<Expr> castToExpr(std::any value);

	std::shared_ptr<Expr> evaluate(std::shared_ptr<Expr> expr);

	std::wstring stringify(std::shared_ptr<Expr> n);

	bool isEqual(std::shared_ptr<Expr> a, std::shared_ptr<Expr> b);

	void checkNumberOperand(std::shared_ptr<Token> operatorX, std::shared_ptr<Expr> operand);

	void checkNumberOperands(std::shared_ptr<Token> operatorX, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right);

	bool isTruthy(std::shared_ptr<Expr> n);

	void execute(std::shared_ptr<Stmt> stmt);

	std::any lookUpVariable(std::shared_ptr<Token> name, std::shared_ptr<Expr> expr);
};
