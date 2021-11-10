#include <string>
#include <iostream>
#include <sstream>

#include "astprinter.h"

std::wstring
AstPrinter::print(Expr<std::wstring> *expr)
{
	return expr->accept(this);
}

std::wstring
AstPrinter::visitBinaryExpr(Binary<std::wstring> *expr)
{
	return parenthesize(expr->m_operatorX->lexeme,
		expr->m_left,
		expr->m_right);
}

std::wstring
AstPrinter::visitGroupingExpr(Grouping<std::wstring> *expr)
{
	std::wstring grouping(L"group");
	return parenthesize(grouping, expr->m_expression);
}

std::wstring
AstPrinter::visitDoubleLiteralExpr(DoubleLiteral<std::wstring> *expr)
{
	std::wostringstream os;
	os << expr->m_value;
	return os.str();
}

std::wstring
AstPrinter::visitStringLiteralExpr(StringLiteral<std::wstring> *expr)
{
	return expr->m_value;
}

std::wstring
AstPrinter::visitBooleanLiteralExpr(BooleanLiteral<std::wstring> *expr)
{
	return expr->m_value ? L"true" : L"false";
}

std::wstring
AstPrinter::visitNilLiteralExpr(NilLiteral<std::wstring> *expr)
{
	return L"nil";
}

std::wstring
AstPrinter::visitUnaryExpr(Unary<std::wstring> *expr)
{
	return parenthesize(expr->m_operatorX->lexeme, expr->m_right);
}

std::wstring
AstPrinter::parenthesize(const std::wstring &name,
	std::shared_ptr<Expr<std::wstring>> expr1)
{
	std::wostringstream os;
	os << "(" << name << " " << expr1->accept(this) << ")";
	return os.str();
}

std::wstring
AstPrinter::parenthesize(const std::wstring &name,
	std::shared_ptr<Expr<std::wstring>> expr1,
	std::shared_ptr<Expr<std::wstring>> expr2)
{
	std::wostringstream os;
	os << "(" << name << " " <<
		expr1->accept(this) << " " <<
		expr2->accept(this) << ")";
	return os.str();
}
