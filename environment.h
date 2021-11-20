#pragma once

#include <any>
#include <map>
#include <string>
#include <memory>

#include "expr.h"

class Environment
{
public:
	Environment();

	Environment(std::shared_ptr<Environment> enclosingEnvironment);

	void define(const std::wstring &name, std::shared_ptr<Expr> value);

	std::shared_ptr<Expr> get(std::shared_ptr<Token> name);

	void assign(std::shared_ptr<Token> name, std::shared_ptr<Expr> value);

private:
	std::map<std::wstring, std::shared_ptr<Expr>> values;
	std::shared_ptr<Environment> enclosing;
};
