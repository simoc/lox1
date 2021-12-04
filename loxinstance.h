#pragma once

#include <memory>

/*
 * Solve circular dependency between classes LoxClass, LoxFunction and LoxInstance.
 */
class LoxClass;

#include "token.h"
#include "expr.h"
#include "loxclass.h"

/**
 * An instance of a class, from Chapter 12.3.
 */
class LoxInstance : public Expr, public std::enable_shared_from_this<LoxInstance>
{
public:
	LoxInstance(std::shared_ptr<LoxClass> _klass);

	std::wstring toString();

	std::any accept(ExprVisitor *visitor);

	std::size_t arity();

	std::shared_ptr<Expr> get(std::shared_ptr<Token> name);

	void set(std::shared_ptr<Token> name, std::shared_ptr<Expr> value);

private:
	std::shared_ptr<LoxClass> klass;

	std::map<std::wstring, std::shared_ptr<Expr>> fields;
};
