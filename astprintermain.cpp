#include <string>
#include <iostream>
#include <sstream>

#include "token.h"
#include "expr.h"
#include "astprinter.h"

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
