#include <sstream>
#include "interpreter.h"
#include "runtimeerror.h"
#include "lox.h"

Interpreter::Interpreter()
{
	environment = std::make_shared<Environment>();
}

std::any
Interpreter::visitAssignExpr(std::shared_ptr<Assign> expr)
{
	std::any value = evaluate(expr->m_value);
	auto valueExpr = std::any_cast<std::shared_ptr<Expr>>(value);
	environment->assign(expr->m_name, valueExpr);
	return value;
}

std::any
Interpreter::visitBlockStmt(std::shared_ptr<Block> stmt)
{
	executeBlock(stmt->m_statements, std::make_shared<Environment>(environment));
	return std::make_shared<NilLiteral>();
}

void
Interpreter::executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> env)
{
	std::shared_ptr<Environment> previous = environment;
	try
	{
		environment = env;

		for (std::shared_ptr<Stmt> statement : statements)
		{
			execute(statement);
		}
	}
	catch (const std::exception &e)
	{
		environment = previous;
		throw e;
	}
}

std::any
Interpreter::visitBinaryExpr(std::shared_ptr<Binary> expr)
{
	std::any left = evaluate(expr->m_left);
	std::any right = evaluate(expr->m_right);

	if (expr->m_operatorX->type == PLUS)
	{
		try
		{
			auto leftString = std::any_cast<std::shared_ptr<StringLiteral>>(left);
			auto rightString = std::any_cast<std::shared_ptr<StringLiteral>>(right);
			return std::make_shared<StringLiteral>(leftString->m_value + rightString->m_value);
		}
		catch (const std::bad_any_cast &e)
		{
			try
			{
				auto leftDouble = std::any_cast<std::shared_ptr<DoubleLiteral>>(left);
				auto rightDouble = std::any_cast<std::shared_ptr<DoubleLiteral>>(right);
				return std::make_shared<DoubleLiteral>(leftDouble->m_value + rightDouble->m_value);
			}
			catch (const std::bad_any_cast &e)
			{
			}
		}
		throw RuntimeError(expr->m_operatorX, L"Operands must be two numbers or two strings.");
	}
	else
	{
		if (expr->m_operatorX->type == BANG_EQUAL)
		{
			return std::make_shared<BooleanLiteral>(!isEqual(left, right));
		}
		else if (expr->m_operatorX->type == EQUAL_EQUAL)
		{
			return std::make_shared<BooleanLiteral>(isEqual(left, right));
		}

		checkNumberOperands(expr->m_operatorX, left, right);

		auto leftDouble = std::any_cast<std::shared_ptr<DoubleLiteral>>(left);
		auto rightDouble = std::any_cast<std::shared_ptr<DoubleLiteral>>(right);

		if (expr->m_operatorX->type == MINUS)
		{
			return std::make_shared<DoubleLiteral>(leftDouble->m_value - rightDouble->m_value);
		}
		else if (expr->m_operatorX->type == SLASH)
		{
			return std::make_shared<DoubleLiteral>(leftDouble->m_value / rightDouble->m_value);
		}
		else if (expr->m_operatorX->type == STAR)
		{
			return std::make_shared<DoubleLiteral>(leftDouble->m_value * rightDouble->m_value);
		}
		else if (expr->m_operatorX->type == GREATER)
		{
			return std::make_shared<BooleanLiteral>(leftDouble->m_value > rightDouble->m_value);
		}
		else if (expr->m_operatorX->type == GREATER_EQUAL)
		{
			return std::make_shared<BooleanLiteral>(leftDouble->m_value >= rightDouble->m_value);
		}
		else if (expr->m_operatorX->type == LESS)
		{
			return std::make_shared<BooleanLiteral>(leftDouble->m_value < rightDouble->m_value);
		}
		else if (expr->m_operatorX->type == LESS_EQUAL)
		{
			return std::make_shared<BooleanLiteral>(leftDouble->m_value <= rightDouble->m_value);
		}
	}

	return std::make_shared<NilLiteral>();
}

std::any
Interpreter::visitGroupingExpr(std::shared_ptr<Grouping> expr)
{
	return evaluate(expr->m_expression);
}

std::any
Interpreter::visitDoubleLiteralExpr(std::shared_ptr<DoubleLiteral> expr)
{
	return expr;
}

std::any
Interpreter::visitStringLiteralExpr(std::shared_ptr<StringLiteral> expr)
{
	return expr;
}

std::any
Interpreter::visitBooleanLiteralExpr(std::shared_ptr<BooleanLiteral> expr)
{
	return expr;
}

std::any
Interpreter::visitNilLiteralExpr(std::shared_ptr<NilLiteral> expr)
{
	return expr;
}

std::any
Interpreter::visitLogicalExpr(std::shared_ptr<Logical> expr)
{
	std::any left = evaluate(expr->m_left);

	if (expr->m_operatorX->type == TokenType::OR)
	{
		if (isTruthy(left))
		{
			return left;
		}
	}
	else
	{
		if (!isTruthy(left))
		{
			return left;
		}
	}
	return evaluate(expr->m_right);
}

std::any
Interpreter::visitUnaryExpr(std::shared_ptr<Unary> expr)
{
	std::any right = evaluate(expr->m_right);
	if (expr->m_operatorX->type == MINUS)
	{
		checkNumberOperand(expr->m_operatorX, right);
		auto ptr = std::any_cast<std::shared_ptr<DoubleLiteral>>(right);
		if (ptr)
		{
			auto neg = std::make_shared<DoubleLiteral>(-ptr->m_value);
			return neg;
		}
	}
	else if (expr->m_operatorX->type == BANG)
	{
		bool b = isTruthy(right);
		return std::make_shared<BooleanLiteral>(!b);
	}

	return std::make_shared<NilLiteral>();
}

std::any
Interpreter::visitVariableExpr(std::shared_ptr<Variable> expr)
{
	std::any value = evaluate(environment->get(expr->m_name));
	return value;
}

std::any
Interpreter::visitIfStmt(std::shared_ptr<If> stmt)
{
	if (isTruthy(evaluate(stmt->m_condition)))
	{
		execute(stmt->m_thenBranch);
	}
	else if (stmt->m_elseBranch)
	{
		execute(stmt->m_elseBranch);
	}
	return nullptr;
}

std::any
Interpreter::visitPrintStmt(std::shared_ptr<Print> stmt)
{
	std::any value = evaluate(stmt->m_expression);
	std::wcout << stringify(value) << std::endl;
	return value;
}

std::any
Interpreter::visitVarStmt(std::shared_ptr<Var> stmt)
{
	if (stmt->m_initializer)
	{
		std::any value = evaluate(stmt->m_initializer);
		try
		{
			// TODO: find a way to do this without trying different data types
			std::shared_ptr<StringLiteral> s = std::any_cast<std::shared_ptr<StringLiteral>>(value);
			environment->define(stmt->m_name->lexeme, s);
		}
		catch (const std::bad_any_cast &e)
		{
			try
			{
				std::shared_ptr<DoubleLiteral> d = std::any_cast<std::shared_ptr<DoubleLiteral>>(value);
				environment->define(stmt->m_name->lexeme, d);
			}
			catch (const std::bad_any_cast &e)
			{
				try
				{
					std::shared_ptr<BooleanLiteral> b = std::any_cast<std::shared_ptr<BooleanLiteral>>(value);
					environment->define(stmt->m_name->lexeme, b);
				}
				catch (const std::bad_any_cast &e)
				{
					std::shared_ptr<NilLiteral> n = std::any_cast<std::shared_ptr<NilLiteral>>(value);
					environment->define(stmt->m_name->lexeme, n);
				}
			}
		}
	}
	else
	{
		environment->define(stmt->m_name->lexeme, std::make_shared<NilLiteral>());
	}
	return std::make_shared<NilLiteral>();
}

std::any
Interpreter::visitExpressionStmt(std::shared_ptr<Expression> stmt)
{
	std::any value = evaluate(stmt->m_expression);
	return value;
}

void
Interpreter::checkNumberOperand(std::shared_ptr<Token> operatorX, std::any operand)
{
	try
	{
		std::any_cast<std::shared_ptr<DoubleLiteral>>(operand);
		return;
	}
	catch (const std::bad_any_cast &e)
	{
	}
	throw RuntimeError(operatorX, L"Operand must be a number.");
}

void
Interpreter::checkNumberOperands(std::shared_ptr<Token> operatorX,
	std::any left,
	std::any right)
{
	try
	{
		std::any_cast<std::shared_ptr<DoubleLiteral>>(left);
		std::any_cast<std::shared_ptr<DoubleLiteral>>(right);
		return;
	}
	catch (const std::bad_any_cast &e)
	{
	}
	throw RuntimeError(operatorX, L"Operands must be numbers.");
}

bool
Interpreter::isTruthy(std::any n)
{
	try
	{
		auto nilValue = std::any_cast<std::shared_ptr<NilLiteral>>(n);
		return false;
	}
	catch (const std::bad_any_cast &e)
	{
		try
		{
			auto booleanValue = std::any_cast<std::shared_ptr<BooleanLiteral>>(n);
			return booleanValue->m_value;
		}
		catch (const std::bad_any_cast &e)
		{
		}
	}
	return true;
}

bool
Interpreter::isEqual(std::any a, std::any b)
{
	try
	{
		std::any_cast<std::shared_ptr<NilLiteral>>(a);
		try
		{
			std::any_cast<std::shared_ptr<NilLiteral>>(b);
		}
		catch (const std::bad_any_cast &e)
		{
			return false;
		}
		return true;
	}
	catch (const std::bad_any_cast &e)
	{
		try
		{
			std::shared_ptr<DoubleLiteral> da = std::any_cast<std::shared_ptr<DoubleLiteral>>(a);
			std::shared_ptr<DoubleLiteral> db = std::any_cast<std::shared_ptr<DoubleLiteral>>(b);
			return (da->m_value == db->m_value);
		}
		catch (const std::bad_any_cast &e)
		{
			try
			{
				std::shared_ptr<StringLiteral> sa = std::any_cast<std::shared_ptr<StringLiteral>>(a);
				std::shared_ptr<StringLiteral> sb = std::any_cast<std::shared_ptr<StringLiteral>>(b);
				return (sa->m_value == sb->m_value);
			}
			catch (const std::bad_any_cast &e)
			{
				try
				{
					std::shared_ptr<BooleanLiteral> ba = std::any_cast<std::shared_ptr<BooleanLiteral>>(a);
					std::shared_ptr<BooleanLiteral> bb = std::any_cast<std::shared_ptr<BooleanLiteral>>(b);
					return (ba->m_value == bb->m_value);
				}
				catch (const std::bad_any_cast &e)
				{
				}
			}
		}
	}
	return false;
}

std::any
Interpreter::evaluate(std::shared_ptr<Expr> expr)
{
	std::any result = expr->accept(this);
	return result;
}

std::wstring
Interpreter::stringify(std::any n)
{
	try
	{
		std::any_cast<std::shared_ptr<NilLiteral>>(n);
		return std::wstring(L"nil");
	}
	catch (const std::bad_any_cast &e)
	{
		try
		{
			std::shared_ptr<StringLiteral> s = std::any_cast<std::shared_ptr<StringLiteral>>(n);
			return s->m_value;
		}
		catch (const std::bad_any_cast &e)
		{
			try
			{
				std::shared_ptr<BooleanLiteral> b = std::any_cast<std::shared_ptr<BooleanLiteral>>(n);
				return b->m_value ? std::wstring(L"true") : std::wstring(L"false");
			}
			catch (const std::bad_any_cast &e)
			{
				try
				{
					std::shared_ptr<DoubleLiteral> d = std::any_cast<std::shared_ptr<DoubleLiteral>>(n);
					std::wostringstream os;
					os << d->m_value;
					return os.str();
				}
				catch (const std::bad_any_cast &e)
				{
				}
			}
		}
	}
	return std::wstring();
}

void
Interpreter::interpret(std::vector<std::shared_ptr<Stmt>> statements)
{
	try
	{
		for (std::shared_ptr<Stmt> statement :  statements)
		{
			execute(statement);
		}
	}
	catch (const RuntimeError &e)
	{
		Lox::runtimeError(e);
	}
}

void
Interpreter::execute(std::shared_ptr<Stmt> stmt)
{
	stmt->accept(this);
}
