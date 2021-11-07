#include <string>
#include <iostream>
#include <sstream>

#include "token.h"
#include "expr.h"

class AstPrinter : public Visitor<std::wstring>
{
public:
	std::wstring print(Expr<std::wstring> *expr)
	{
		return expr->accept(this);
	}

	std::wstring visitBinaryExpr(Binary<std::wstring> *expr)
	{
		return parenthesize(expr->m_operatorX->lexeme,
			expr->m_left,
			expr->m_right);
	}
	std::wstring visitGroupingExpr(Grouping<std::wstring> *expr)
	{
		std::wstring grouping(L"group");
		return parenthesize(grouping, expr->m_expression);
	}
	std::wstring visitDoubleLiteralExpr(DoubleLiteral<std::wstring> *expr)
	{
		std::wostringstream os;
		os << expr->m_value;
		return os.str();
	}
	std::wstring visitStringLiteralExpr(StringLiteral<std::wstring> *expr)
	{
		return expr->m_value;
	}
	std::wstring visitUnaryExpr(Unary<std::wstring> *expr)
	{
		return parenthesize(expr->m_operatorX->lexeme, expr->m_right);
	}
	std::wstring parenthesize(const std::wstring &name, Expr<std::wstring> *expr1)
	{
		std::wostringstream os;
		os << "(" << name << " " << expr1->accept(this) << ")";
		return os.str();
	}
	std::wstring parenthesize(const std::wstring &name,
		Expr<std::wstring> *expr1, Expr<std::wstring> *expr2)
	{
		std::wostringstream os;
		os << "(" << name << " " <<
			expr1->accept(this) << " " <<
			expr2->accept(this) << ")";
		return os.str();
	}
};

int
main(int argc, char *argv[])
{
	DoubleLiteral<std::wstring> d1(123);
	Token t1(TokenType::MINUS, L"-", 1);
	Unary<std::wstring> u1(&t1, &d1);
	Token t2(TokenType::STAR, L"*", 1);
	DoubleLiteral<std::wstring> d2(45.67);
	Grouping<std::wstring> g(&d2);

	auto expression = Binary<std::wstring>(
		&u1,
		&t2,
		&g);

	AstPrinter printer;
	std::wcout << printer.print(&expression) << std::endl;
}
