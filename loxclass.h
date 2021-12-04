#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

/*
 * Solve circular dependency between classes LoxClass, LoxFunction and LoxInstance.
 */
class LoxFunction;

#include "expr.h"
#include "loxcallable.h"
#include "interpreter.h"
#include "loxfunction.h"

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass>
{
public:
	LoxClass(const std::wstring &_name, const std::map<std::wstring, std::shared_ptr<LoxFunction>> &_methods);

	std::wstring toString() const;

	std::any accept(ExprVisitor *visitor);

	std::size_t arity();

	std::shared_ptr<Expr> call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments);

	std::shared_ptr<LoxFunction> findMethod(const std::wstring &name);

private:
	std::wstring name;

	std::map<std::wstring, std::shared_ptr<LoxFunction>> methods;
};
