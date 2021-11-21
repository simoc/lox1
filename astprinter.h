#pragma once

#include <string>
#include <memory>

#include "token.h"
#include "expr.h"

class AstPrinter : public ExprVisitor
{
public:
	std::wstring print(std::shared_ptr<Expr> expr);

	std::any visitAssignExpr(std::shared_ptr<Assign> expr);

	std::any visitBinaryExpr(std::shared_ptr<Binary> expr);

	std::any visitGroupingExpr(std::shared_ptr<Grouping> expr);

	std::any visitDoubleLiteralExpr(std::shared_ptr<DoubleLiteral> expr);

	std::any visitStringLiteralExpr(std::shared_ptr<StringLiteral> expr);

	std::any visitBooleanLiteralExpr(std::shared_ptr<BooleanLiteral> expr);

	std::any visitNilLiteralExpr(std::shared_ptr<NilLiteral> expr);

	std::any visitLogicalExpr(std::shared_ptr<Logical> expr);

	std::any visitUnaryExpr(std::shared_ptr<Unary> expr);

	std::any visitVariableExpr(std::shared_ptr<Variable> expr);

	std::any parenthesize(const std::wstring &name, std::shared_ptr<Expr> expr1);

	std::any parenthesize(const std::wstring &name,
		std::shared_ptr<Expr> expr1,
		std::shared_ptr<Expr> expr2);
};
