#include "resolver.h"
#include "lox.h"

Resolver::Resolver(Interpreter &_interpreter) :
	interpreter(_interpreter)
{
}

std::any
Resolver::visitAssignExpr(std::shared_ptr<Assign> expr)
{
	resolve(expr->m_value);
	resolveLocal(expr, expr->m_name);
	return nullptr;
}

std::any
Resolver::visitBinaryExpr(std::shared_ptr<Binary> expr)
{
	resolve(expr->m_left);
	resolve(expr->m_right);
	return nullptr;
}

std::any
Resolver::visitCallExpr(std::shared_ptr<Call> expr)
{
	resolve(expr->m_callee);
	for (std::shared_ptr<Expr> argument : expr->m_arguments)
	{
		resolve(argument);
	}
	return nullptr;
}

std::any
Resolver::visitGroupingExpr(std::shared_ptr<Grouping> expr)
{
	resolve(expr->m_expression);
	return nullptr;
}

std::any
Resolver::visitDoubleLiteralExpr(std::shared_ptr<DoubleLiteral> expr)
{
	return nullptr;
}

std::any
Resolver::visitStringLiteralExpr(std::shared_ptr<StringLiteral> expr)
{
	return nullptr;
}

std::any
Resolver::visitBooleanLiteralExpr(std::shared_ptr<BooleanLiteral> expr)
{
	return nullptr;
}

std::any
Resolver::visitNilLiteralExpr(std::shared_ptr<NilLiteral> expr)
{
	return nullptr;
}

std::any
Resolver::visitLogicalExpr(std::shared_ptr<Logical> expr)
{
	resolve(expr->m_left);
	resolve(expr->m_right);
	return nullptr;
}

std::any
Resolver::visitUnaryExpr(std::shared_ptr<Unary> expr)
{
	resolve(expr->m_right);
	return nullptr;
}

std::any
Resolver::visitVariableExpr(std::shared_ptr<Variable> expr)
{
	if (!scopes.empty())
	{
		auto it = scopes.back().find(expr->m_name->lexeme);
		if (it != scopes.back().end())
		{
			if (it->second == false)
			{
				Lox::error(expr->m_name,
					L"Can't read local variable in its own initializer.");
			}

		}
	}
	resolveLocal(expr, expr->m_name);
	return nullptr;
}

std::any
Resolver::visitIfStmt(std::shared_ptr<If> stmt)
{
	resolve(stmt->m_condition);
	resolve(stmt->m_thenBranch);
	if (stmt->m_elseBranch)
		resolve(stmt->m_elseBranch);
	return nullptr;
}

std::any
Resolver::visitFunctionStmt(std::shared_ptr<Function> stmt)
{
	declare(stmt->m_name);
	define(stmt->m_name);
	resolveFunction(stmt, FunctionType::FUNCTION);
	return nullptr;
}

std::any
Resolver::visitPrintStmt(std::shared_ptr<Print> stmt)
{
	resolve(stmt->m_expression);
	return nullptr;
}

std::any
Resolver::visitReturnStmt(std::shared_ptr<Return> stmt)
{
	if (currentFunction == FunctionType::NONE)
	{
		Lox::error(stmt->m_keyword, L"Can't return from top-level code.");
	}

	if (stmt->m_value)
	{
		resolve(stmt->m_value);
	}
	return nullptr;
}

std::any
Resolver::visitExpressionStmt(std::shared_ptr<Expression> stmt)
{
	resolve(stmt->m_expression);
	return nullptr;
}

std::any
Resolver::visitBlockStmt(std::shared_ptr<Block> stmt)
{
	beginScope();
	resolve(stmt->m_statements);
	endScope();
	return nullptr;
}

std::any
Resolver::visitClassStmt(std::shared_ptr<Class> stmt)
{
	declare(stmt->m_name);
	define(stmt->m_name);
	return nullptr;
}

std::any
Resolver::visitVarStmt(std::shared_ptr<Var> stmt)
{
	declare(stmt->m_name);
	if (stmt->m_initializer)
	{
		resolve(stmt->m_initializer);
	}
	define(stmt->m_name);
	return nullptr;
}

std::any
Resolver::visitWhileStmt(std::shared_ptr<While> stmt)
{
	resolve(stmt->m_condition);
	resolve(stmt->m_body);
	return nullptr;
}

void
Resolver::resolve(const std::vector<std::shared_ptr<Stmt>> &statements)
{
	for (const auto &statement : statements)
	{
		resolve(statement);
	}
}

void
Resolver::resolve(std::shared_ptr<Stmt> statement)
{
	statement->accept(this);
}

void
Resolver::resolve(std::shared_ptr<Expr> expr)
{
	expr->accept(this);
}

void
Resolver::beginScope()
{
	scopes.push_back(std::map<std::wstring, bool>());
}

void
Resolver::endScope()
{
	scopes.pop_back();
}

void
Resolver::declare(std::shared_ptr<Token> name)
{
	if (scopes.empty())
	{
		return;
	}

	auto it = scopes.back().find(name->lexeme);
	if (it != scopes.back().end())
	{
		Lox::error(name, L"Already a variable with this name in this scope.");
	}
	scopes.back().insert_or_assign(name->lexeme, false);
}

void
Resolver::define(std::shared_ptr<Token> name)
{
	if (scopes.empty())
	{
		return;
	}

	scopes.back().insert_or_assign(name->lexeme, true);
}

void
Resolver::resolveLocal(std::shared_ptr<Expr> expr, std::shared_ptr<Token> name)
{
	for (int i = scopes.size() - 1; i >= 0; i--)
	{
		auto it = scopes.at(i).find(name->lexeme);
		if (it != scopes.at(i).end())
		{
			interpreter.resolve(expr, scopes.size() - 1 - i);
			return;
		}
	}
}

void
Resolver::resolveFunction(std::shared_ptr<Function> func, FunctionType type)
{
	FunctionType enclosingFunction = currentFunction;
	currentFunction = type;

	beginScope();
	for (std::shared_ptr<Token> param : func->m_params)
	{
		declare(param);
		define(param);
	}
	resolve(func->m_body);
	endScope();

	currentFunction = enclosingFunction;
}
