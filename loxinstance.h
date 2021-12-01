#pragma once

#include "loxclass.h"

/**
 * An instance of a class, from Chapter 12.3.
 */
class LoxInstance : public Expr
{
public:
	LoxInstance(std::shared_ptr<LoxClass> _klass);

	std::wstring toString();

	std::any accept(ExprVisitor *visitor);

	std::size_t arity();

private:
	std::shared_ptr<LoxClass> klass;
};
