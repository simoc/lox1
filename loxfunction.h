#pragma once

#include <memory>

/*
 * Solve circular dependency between classes LoxClass, LoxFunction and LoxInstance.
 */
class LoxInstance;

#include "loxcallable.h"
#include "loxinstance.h"
#include "environment.h"

class LoxFunction : public LoxCallable
{
public:
	LoxFunction(std::shared_ptr<Function> _declaration, std::shared_ptr<Environment> _closure,
		bool _isInitializer);

	std::size_t arity();

	std::shared_ptr<Expr> call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments);

	std::any accept(ExprVisitor *visitor);

	std::wstring toString();

	std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> inst);

private:
	std::shared_ptr<Function> declaration;

	std::shared_ptr<Environment> closure;

	bool isInitializer;
};
