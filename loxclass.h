#pragma once

#include <string>

#include "expr.h"

class LoxClass : public Expr
{
public:
	LoxClass(const std::wstring &_name);

	std::wstring toString() const;

	std::any accept(ExprVisitor *visitor);

private:
	std::wstring name;
};
