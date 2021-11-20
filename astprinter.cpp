#include <string>
#include <iostream>
#include <sstream>

#include "astprinter.h"

std::wstring
AstPrinter::print(std::shared_ptr<Expr> expr)
{
	std::any result = expr->accept(this);
	return std::any_cast<std::wstring>(result);
}

std::any
AstPrinter::visitAssignExpr(std::shared_ptr<Assign> expr)
{
	std::wostringstream os;
	os << "assign " << expr->m_name->lexeme;
	return parenthesize(os.str(), expr->m_value);
}

std::any
AstPrinter::visitBinaryExpr(std::shared_ptr<Binary> expr)
{
	return parenthesize(expr->m_operatorX->lexeme,
		expr->m_left,
		expr->m_right);
}

std::any
AstPrinter::visitGroupingExpr(std::shared_ptr<Grouping> expr)
{
	std::wstring grouping(L"group");
	return parenthesize(grouping, expr->m_expression);
}

std::any
AstPrinter::visitDoubleLiteralExpr(std::shared_ptr<DoubleLiteral> expr)
{
	std::wostringstream os;
	os << expr->m_value;
	return os.str();
}

std::any
AstPrinter::visitStringLiteralExpr(std::shared_ptr<StringLiteral> expr)
{
	return expr->m_value;
}

std::any
AstPrinter::visitBooleanLiteralExpr(std::shared_ptr<BooleanLiteral> expr)
{
	return expr->m_value ? std::wstring(L"true") : std::wstring(L"false");
}

std::any
AstPrinter::visitNilLiteralExpr(std::shared_ptr<NilLiteral> expr)
{
	return std::wstring(L"nil");
}

std::any
AstPrinter::visitUnaryExpr(std::shared_ptr<Unary> expr)
{
	return parenthesize(expr->m_operatorX->lexeme, expr->m_right);
}

std::any
AstPrinter::visitVariableExpr(std::shared_ptr<Variable> expr)
{
	return expr->m_name->lexeme;
}

std::any
AstPrinter::parenthesize(const std::wstring &name,
	std::shared_ptr<Expr> expr1)
{
	std::wostringstream os;
	std::any result = expr1->accept(this);
	os << "(" << name << " " << std::any_cast<std::wstring>(result) << ")";
	return os.str();
}

std::any
AstPrinter::parenthesize(const std::wstring &name,
	std::shared_ptr<Expr> expr1,
	std::shared_ptr<Expr> expr2)
{
	std::wostringstream os;
	std::any result1 = expr1->accept(this);
	std::any result2 = expr2->accept(this);
	os << "(" << name << " " <<
		std::any_cast<std::wstring>(result1) << " " <<
		std::any_cast<std::wstring>(result2) << ")";
	return os.str();
}
