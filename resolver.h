#pragma once

#include <map>
#include <vector>

#include "expr.h"
#include "stmt.h"
#include "interpreter.h"

/*
 * Indicates whether resolver is currently in a function, or not.
 * From 11.5, Resolution Errors.
 */
enum class FunctionType
{
	NONE,
	FUNCTION
};

/**
 * Resolves lox programs, from Chapter 11.
 */
class Resolver : public ExprVisitor, public StmtVisitor
{
public:
	Resolver(Interpreter &_interpreter);

	std::any visitAssignExpr(std::shared_ptr<Assign> expr);

	std::any visitBlockStmt(std::shared_ptr<Block> stmt);

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

	std::any visitPrintStmt(std::shared_ptr<Print> stmt);

	std::any visitReturnStmt(std::shared_ptr<Return> stmt);

	std::any visitExpressionStmt(std::shared_ptr<Expression> stmt);

	std::any visitVarStmt(std::shared_ptr<Var> stmt);

	std::any visitWhileStmt(std::shared_ptr<While> stmt);

	void resolve(const std::vector<std::shared_ptr<Stmt>> &statements);

private:
	Interpreter &interpreter;
	std::vector<std::map<std::wstring, bool>> scopes;
	FunctionType currentFunction = FunctionType::NONE;

	void resolve(std::shared_ptr<Stmt> statement);

	void resolve(std::shared_ptr<Expr> expr);

	void beginScope();

	void endScope();

	void declare(std::shared_ptr<Token> name);

	void define(std::shared_ptr<Token> name);

	void resolveLocal(std::shared_ptr<Expr> expr, std::shared_ptr<Token> name);

	void resolveFunction(std::shared_ptr<Function> func, FunctionType type);
};
