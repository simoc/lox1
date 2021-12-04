#include "loxfunction.h"
#include "returnerror.h"

LoxFunction::LoxFunction(std::shared_ptr<Function> _declaration,
	std::shared_ptr<Environment> _closure,
	bool _isInitializer) :
	declaration(_declaration),
	closure(_closure),
	isInitializer(_isInitializer)
{
}

std::size_t
LoxFunction::arity()
{
	return declaration->m_params.size();
}

std::shared_ptr<Expr>
LoxFunction::call(Interpreter *interpreter, std::vector<std::shared_ptr<Expr>> arguments)
{
	std::shared_ptr<Environment> environment = std::make_shared<Environment>(closure);
	for (std::size_t i = 0; i < declaration->m_params.size(); i++)
	{
		environment->define(declaration->m_params.at(i)->lexeme, arguments.at(i));
	}
	try
	{
		interpreter->executeBlock(declaration->m_body, environment);
	}
	catch (const ReturnError &e)
	{
		return e.value;
	}

	if (isInitializer)
	{
		return closure->getAt(0, L"this");
	}
	return std::make_shared<NilLiteral>();
}

std::any
LoxFunction::accept(ExprVisitor *visitor)
{
	return std::make_shared<NilLiteral>();
}

std::wstring
LoxFunction::toString()
{
	return L"<fn " + declaration->m_name->lexeme + L">";
}

std::shared_ptr<LoxFunction>
LoxFunction::bind(std::shared_ptr<LoxInstance> inst)
{
	std::shared_ptr<Environment> environment = std::make_shared<Environment>(closure);
	environment->define(L"this", inst);
	return std::make_shared<LoxFunction>(declaration, environment, isInitializer);
}
