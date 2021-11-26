#include <chrono>

#include "clockfunction.h"

std::size_t
ClockFunction::arity()
{
	return 0;
}

std::shared_ptr<Expr>
ClockFunction::call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments)
{
	auto seconds = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	return std::make_shared<DoubleLiteral>(seconds);
}

std::any
ClockFunction::accept(ExprVisitor *visitor)
{
	return std::make_shared<NilLiteral>();
}
