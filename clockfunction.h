#pragma once

#include "loxcallable.h"

/**
 * Built-in clock() function from 10.2.1.
 */
class ClockFunction : public LoxCallable
{
public:
	std::size_t arity();

	std::shared_ptr<Expr> call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments);

	std::any accept(ExprVisitor *visitor);
};
