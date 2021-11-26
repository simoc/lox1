#pragma once

#include "loxcallable.h"

class LoxFunction : public LoxCallable
{
public:
	LoxFunction(std::shared_ptr<Function> _declaration);

	std::size_t arity();

	std::shared_ptr<Expr> call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments);

	std::any accept(ExprVisitor *visitor);
private:
	std::shared_ptr<Function> declaration;
};
