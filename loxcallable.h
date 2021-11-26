#pragma once

#include <memory>
#include <vector>

#include "expr.h"
#include "interpreter.h"

class LoxCallable : public Expr
{
public:
	virtual std::size_t arity() = 0;

	virtual std::shared_ptr<Expr> call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments) = 0;
};
