#include <string>
#include <iostream>
#include <sstream>
#include <memory>

#include "token.h"
#include "expr.h"
#include "astprinter.h"

int
main(int argc, char *argv[])
{
	auto d1 = std::make_shared<DoubleLiteral<std::wstring>>(123);
	auto t1 = std::make_shared<Token>(TokenType::MINUS, L"-", 1);
	auto u1 = std::make_shared<Unary<std::wstring>>(t1, d1);

	auto t2 = std::make_shared<Token>(TokenType::STAR, L"*", 1);

	auto d2 = std::make_shared<DoubleLiteral<std::wstring>>(45.67);
	auto g = std::make_shared<Grouping<std::wstring>>(d2);

	auto expression = Binary<std::wstring>(
		u1,
		t2,
		g);

	AstPrinter printer;
	std::wcout << printer.print(&expression) << std::endl;
}
