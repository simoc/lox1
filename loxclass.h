#pragma once

#include <string>
#include <memory>
#include <vector>

#include "expr.h"
#include "loxcallable.h"
#include "interpreter.h"

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass>
{
public:
	LoxClass(const std::wstring &_name);

	std::wstring toString() const;

	std::any accept(ExprVisitor *visitor);

	std::size_t arity();

	std::shared_ptr<Expr> call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments);

private:
	std::wstring name;
};
