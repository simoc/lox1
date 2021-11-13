#pragma once

#include <string>
#include <memory>

#include "token.h"
#include "expr.h"

class AstPrinter : public Visitor<std::wstring>
{
public:
	std::wstring print(std::shared_ptr<Expr<std::wstring>> expr);

	std::wstring visitBinaryExpr(Binary<std::wstring> *expr);

	std::wstring visitGroupingExpr(Grouping<std::wstring> *expr);

	std::wstring visitDoubleLiteralExpr(DoubleLiteral<std::wstring> *expr);

	std::wstring visitStringLiteralExpr(StringLiteral<std::wstring> *expr);

	std::wstring visitBooleanLiteralExpr(BooleanLiteral<std::wstring> *expr);

	std::wstring visitNilLiteralExpr(NilLiteral<std::wstring> *expr);

	std::wstring visitUnaryExpr(Unary<std::wstring> *expr);

	std::wstring parenthesize(const std::wstring &name, std::shared_ptr<Expr<std::wstring>> expr1);

	std::wstring parenthesize(const std::wstring &name,
		std::shared_ptr<Expr<std::wstring>> expr1,
		std::shared_ptr<Expr<std::wstring>> expr2);
};
