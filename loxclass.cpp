#include "loxclass.h"

LoxClass::LoxClass(const std::wstring &_name) :
		name(_name)
{
}

std::wstring
LoxClass::toString() const
{
	return name;
}

std::any
LoxClass::accept(ExprVisitor *visitor)
{
	return std::make_shared<NilLiteral>();
}
