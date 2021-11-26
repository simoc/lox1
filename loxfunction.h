#pragma once

#include <memory>

#include "loxcallable.h"
#include "environment.h"

class LoxFunction : public LoxCallable
{
public:
	LoxFunction(std::shared_ptr<Function> _declaration, std::shared_ptr<Environment> _closure);

	std::size_t arity();

	std::shared_ptr<Expr> call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments);

	std::any accept(ExprVisitor *visitor);

	std::wstring toString();
private:
	std::shared_ptr<Function> declaration;

	std::shared_ptr<Environment> closure;
};
