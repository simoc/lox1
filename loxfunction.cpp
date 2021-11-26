#include "loxfunction.h"


LoxFunction::LoxFunction(std::shared_ptr<Function> _declaration) :
	declaration(_declaration)
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
	std::shared_ptr<Environment> environment = std::make_shared<Environment>(interpreter->globals);
	for (std::size_t i = 0; i < declaration->m_params.size(); i++)
	{
		environment->define(declaration->m_params.at(i)->lexeme, arguments.at(i));
	}
	interpreter->executeBlock(declaration->m_body, environment);
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
