#include "loxinstance.h"

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> _klass) :
	klass(_klass)
{
}

std::wstring
LoxInstance::toString()
{
	return klass->toString() + L" instance";
}

std::any
LoxInstance::accept(ExprVisitor *visitor)
{
	return std::make_shared<NilLiteral>();
}

std::size_t
LoxInstance::arity()
{
	return 0;
}
