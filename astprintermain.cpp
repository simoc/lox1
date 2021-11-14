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
	auto d1 = std::make_shared<DoubleLiteral>(123);
	auto t1 = std::make_shared<Token>(TokenType::MINUS, L"-", 1);
	auto u1 = std::make_shared<Unary>(t1, d1);

	auto t2 = std::make_shared<Token>(TokenType::STAR, L"*", 1);

	auto d2 = std::make_shared<DoubleLiteral>(45.67);
	auto g = std::make_shared<Grouping>(d2);

	auto expression = std::make_shared<Binary>(
		u1,
		t2,
		g);

	AstPrinter printer;
	std::wcout << printer.print(expression) << std::endl;
}
