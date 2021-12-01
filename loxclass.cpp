#include "loxclass.h"
#include "loxinstance.h"

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

std::size_t
LoxClass::arity()
{
	return 0;
}

std::shared_ptr<Expr>
LoxClass::call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments)
{
	std::shared_ptr<LoxInstance> instance = std::make_shared<LoxInstance>(shared_from_this());
	return instance;
}
