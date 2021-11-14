#pragma once

#include <string>
#include <memory>

#include "token.h"
#include "expr.h"

class AstPrinter : public Visitor
{
public:
	std::wstring print(std::shared_ptr<Expr> expr);

	std::any visitBinaryExpr(Binary *expr);

	std::any visitGroupingExpr(Grouping *expr);

	std::any visitDoubleLiteralExpr(DoubleLiteral *expr);

	std::any visitStringLiteralExpr(StringLiteral *expr);

	std::any visitBooleanLiteralExpr(BooleanLiteral *expr);

	std::any visitNilLiteralExpr(NilLiteral *expr);

	std::any visitUnaryExpr(Unary *expr);

	std::any parenthesize(const std::wstring &name, std::shared_ptr<Expr> expr1);

	std::any parenthesize(const std::wstring &name,
		std::shared_ptr<Expr> expr1,
		std::shared_ptr<Expr> expr2);
};
