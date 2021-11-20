#include "environment.h"
#include "runtimeerror.h"

Environment::Environment()
{
};

Environment::Environment(std::shared_ptr<Environment> enclosingEnvironment)
{
	enclosing = enclosingEnvironment;
};

void
Environment::define(const std::wstring &name, std::shared_ptr<Expr> value)
{
	values.insert_or_assign(name, value);
}

std::shared_ptr<Expr>
Environment::get(std::shared_ptr<Token> name)
{
	auto it = values.find(name->lexeme);

	if (it != values.end())
	{
		return it->second;
	}

	if (enclosing)
	{
		return enclosing->get(name);
	}

	throw RuntimeError(name, L"Undefined variable '" + name->lexeme + L"'.");
}

void
Environment::assign(std::shared_ptr<Token> name, std::shared_ptr<Expr> value)
{
	auto it = values.find(name->lexeme);
	if (it != values.end())
	{
		values.insert_or_assign(name->lexeme, value);
		return;
	}

	if (enclosing)
	{
		enclosing->assign(name, value);
		return;
	}

	throw RuntimeError(name, L"Undefined variable '" + name->lexeme + L"'.");
}
