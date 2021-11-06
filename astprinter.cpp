#include <string>
#include <iostream>
#include <sstream>

#include "token.h"
#include "expr.h"

class AstPrinter : public Expr::Visitor<std::wstring>
{
public:
	std::wstring print(Expr &expr)
	{
		return expr.accept(this);
	}

	std::wstring visitBinaryExpr(Binary &expr)
	{
		return parenthesize(expr.m_operatorX.lexeme,
			expr.m_left,
			expr.m_right);
	}
	std::wstring visitGroupingExpr(Grouping &expr)
	{
		std::wstring grouping(L"grouping");
		return parenthesize(grouping, expr.m_expression);
	}
	std::wstring visitDoubleLiteralExpr(DoubleLiteral &expr)
	{
		std::wostringstream os;
		os << expr.m_value;
		return os.str();
	}
	std::wstring visitStringLiteralExpr(StringLiteral &expr)
	{
		return expr.m_value;
	}
	std::wstring visitUnaryExpr(Unary &expr)
	{
		return parenthesize(expr.m_operatorX.lexeme, expr.m_right);
	}
	std::wstring parenthesize(const std::wstring &name, Expr &expr1)
	{
		std::wostringstream os;
		os << "(" << name << " " << expr1.accept(this) << ")";
		return os.str();
	}
	std::wstring parenthesize(const std::wstring &name,
		Expr &expr1, Expr &expr2)
	{
		std::wostringstream os;
		os << "(" << name << " " <<
		       	expr1.accept(this) << " " <<
			expr2.accept(this) << ")";
		return os.str();
	}
};

int
main(int argc, char *argv[])
{
	DoubleLiteral d1(123);
	Token t1(TokenType::MINUS, L"-", 1);
	Unary u1(t1, d1);
	Token t2(TokenType::STAR, L"*", 1);
	DoubleLiteral d2(45.67);
	Grouping g(d2);

	Expr expression = Binary(
		u1,
		t2,
		g);

	AstPrinter printer;
	std::wcout << printer.print(expression) << std::endl;
}
