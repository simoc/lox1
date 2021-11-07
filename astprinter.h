#pragma once

#include <string>

#include "token.h"
#include "expr.h"

class AstPrinter : public Visitor<std::wstring>
{
public:
	std::wstring print(Expr<std::wstring> *expr);

	std::wstring visitBinaryExpr(Binary<std::wstring> *expr);

	std::wstring visitGroupingExpr(Grouping<std::wstring> *expr);

	std::wstring visitDoubleLiteralExpr(DoubleLiteral<std::wstring> *expr);

	std::wstring visitStringLiteralExpr(StringLiteral<std::wstring> *expr);

	std::wstring visitUnaryExpr(Unary<std::wstring> *expr);

	std::wstring parenthesize(const std::wstring &name, Expr<std::wstring> *expr1);

	std::wstring parenthesize(const std::wstring &name,
		Expr<std::wstring> *expr1, Expr<std::wstring> *expr2);
};
