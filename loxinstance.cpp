#include "loxinstance.h"
#include "runtimeerror.h"

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

std::shared_ptr<Expr>
LoxInstance::get(std::shared_ptr<Token> name)
{
	auto it = fields.find(name->lexeme);
	if (it != fields.end())
	{
		return it->second;
	}

	auto method = klass->findMethod(name->lexeme);
	if (method)
	{
		return method->bind(shared_from_this());
	}

	throw RuntimeError(name, L"Undefined property '" + name->lexeme + L"'.");
}

void
LoxInstance::set(std::shared_ptr<Token> name, std::shared_ptr<Expr> value)
{
	fields.insert_or_assign(name->lexeme, value);
}
