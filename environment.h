#pragma once

#include <any>
#include <map>
#include <string>
#include <memory>

#include "expr.h"

class Environment : public std::enable_shared_from_this<Environment>
{
public:
	Environment();

	Environment(std::shared_ptr<Environment> enclosingEnvironment);

	void define(const std::wstring &name, std::shared_ptr<Expr> value);

	std::shared_ptr<Expr> get(std::shared_ptr<Token> name);

	std::shared_ptr<Expr> getAt(int distance, const std::wstring &name);

	void assign(std::shared_ptr<Token> name, std::shared_ptr<Expr> value);

	void assignAt(int distance, std::shared_ptr<Token> name, std::shared_ptr<Expr> value);

	std::shared_ptr<Environment> getEnclosing();

private:
	std::map<std::wstring, std::shared_ptr<Expr>> values;
	std::shared_ptr<Environment> enclosing;

	std::shared_ptr<Environment> ancestor(int distance);
};
