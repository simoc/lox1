#include <sstream>
#include "interpreter.h"
#include "runtimeerror.h"
#include "lox.h"
#include "clockfunction.h"
#include "loxfunction.h"
#include "returnerror.h"
#include "loxclass.h"
#include "loxinstance.h"

Interpreter::Interpreter()
{
	globals = std::make_shared<Environment>();
	environment = globals;

	globals->define(L"clock", std::make_shared<ClockFunction>());
}

std::any
Interpreter::visitAssignExpr(std::shared_ptr<Assign> expr)
{
	auto valueExpr = evaluate(expr->m_value);

	auto it = locals.find(expr);
	if (it != locals.end())
	{
		int distance = it->second;
		environment->assignAt(distance, expr->m_name, valueExpr);
	}
	else
	{
		globals->assign(expr->m_name, valueExpr);
	}
	return valueExpr;
}

std::any
Interpreter::visitBlockStmt(std::shared_ptr<Block> stmt)
{
	executeBlock(stmt->m_statements, std::make_shared<Environment>(environment));
	return std::make_shared<NilLiteral>();
}

std::any
Interpreter::visitClassStmt(std::shared_ptr<Class> stmt)
{
	std::shared_ptr<LoxClass> superclass;
	if (stmt->m_superclass)
	{
		auto expr = evaluate(stmt->m_superclass);
		superclass = std::dynamic_pointer_cast<LoxClass>(expr);
		if (!superclass)
		{
			throw RuntimeError(stmt->m_superclass->m_name,
				L"Superclass must be a class.");
		}
	}
	environment->define(stmt->m_name->lexeme, std::make_shared<NilLiteral>());

	if (stmt->m_superclass)
	{
		environment = std::make_shared<Environment>(environment);
		environment->define(L"super", superclass);
	}

	std::map<std::wstring, std::shared_ptr<LoxFunction>> methods;
	for (std::shared_ptr<Function> method : stmt->m_methods)
	{
		bool isInitializer = (method->m_name->lexeme == std::wstring(L"this"));
		std::shared_ptr<LoxFunction> func = std::make_shared<LoxFunction>(method,
			environment, isInitializer);
		methods.insert_or_assign(method->m_name->lexeme, func);
	}
	std::shared_ptr<LoxClass> klass = std::make_shared<LoxClass>(stmt->m_name->lexeme,
		superclass, methods);

	if (superclass)
	{
		environment = environment->getEnclosing();
	}

	environment->assign(stmt->m_name, klass);
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

		environment = previous;
	}
	catch (const ReturnError &e)
	{
		environment = previous;
		throw e;
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
	std::shared_ptr<Expr> left = evaluate(expr->m_left);
	std::shared_ptr<Expr> right = evaluate(expr->m_right);

	if (expr->m_operatorX->type == PLUS)
	{
		try
		{
			auto leftString = std::dynamic_pointer_cast<StringLiteral>(left);
			auto rightString = std::dynamic_pointer_cast<StringLiteral>(right);
			if (leftString && rightString)
			{
				return std::make_shared<StringLiteral>(leftString->m_value + rightString->m_value);
			}

			auto leftDouble = std::dynamic_pointer_cast<DoubleLiteral>(left);
			auto rightDouble = std::dynamic_pointer_cast<DoubleLiteral>(right);
			if (leftDouble && rightDouble)
			{
				return std::make_shared<DoubleLiteral>(leftDouble->m_value + rightDouble->m_value);
			}
		}
		catch (const std::bad_any_cast &e)
		{
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

		auto leftDouble = std::dynamic_pointer_cast<DoubleLiteral>(left);
		auto rightDouble = std::dynamic_pointer_cast<DoubleLiteral>(right);

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
Interpreter::visitCallExpr(std::shared_ptr<Call> expr)
{
	std::shared_ptr<Expr> callee = evaluate(expr->m_callee);

	std::vector<std::shared_ptr<Expr>> arguments;
	for (std::shared_ptr<Expr> argument : expr->m_arguments)
	{
		arguments.push_back(castToExpr(evaluate(argument)));
	}

	std::shared_ptr<LoxCallable> func;
	try
	{
		func = std::dynamic_pointer_cast<LoxCallable>(callee);
		if (!func)
		{
			throw RuntimeError(expr->m_paren, L"Can only call functions and classes.");
		}
	}
	catch (const std::bad_any_cast &e)
	{
		throw RuntimeError(expr->m_paren, L"Can only call functions and classes.");
	}

	if (arguments.size() != func->arity())
	{
		std::wostringstream os;
		os << L"Expected " << func->arity() << L" arguments but got " <<
			arguments.size() << L".";
		throw RuntimeError(expr->m_paren, os.str());
	}

	return func->call(this, arguments);
}

std::any
Interpreter::visitGetExpr(std::shared_ptr<Get> expr)
{
	std::shared_ptr<Expr> obj = evaluate(expr->m_object);
	std::shared_ptr<LoxInstance> inst = std::dynamic_pointer_cast<LoxInstance>(obj);
	if (inst)
	{
		return inst->get(expr->m_name);
	}

	throw RuntimeError(expr->m_name, L"Only instances have properties.");
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
	std::shared_ptr<Expr> left = evaluate(expr->m_left);

	if (expr->m_operatorX->type == TokenType::OR)
	{
		if (isTruthy(left))
		{
			return left;
		}
	}
	else // TOkenType::AND
	{
		if (!isTruthy(left))
		{
			return left;
		}
	}
	return evaluate(expr->m_right);
}

std::any
Interpreter::visitSetExpr(std::shared_ptr<Set> expr)
{
	std::shared_ptr<Expr> obj = evaluate(expr->m_object);
	std::shared_ptr<LoxInstance> inst = std::dynamic_pointer_cast<LoxInstance>(obj);
	if (!inst)
	{
		throw RuntimeError(expr->m_name, L"Only instances have fields.");
	}

	std::shared_ptr<Expr> value = evaluate(expr->m_value);
	inst->set(expr->m_name, value);
	return value;
}

std::any
Interpreter::visitSuperExpr(std::shared_ptr<Super> expr)
{
	int distance = locals.find(expr)->second;
	std::shared_ptr<Expr> superExpr = environment->getAt(distance, L"super");
	std::shared_ptr<LoxClass> superclass = std::dynamic_pointer_cast<LoxClass>(superExpr);

	std::shared_ptr<Expr> thisExpr = environment->getAt(distance - 1, L"this");
	std::shared_ptr<LoxInstance> obj = std::dynamic_pointer_cast<LoxInstance>(thisExpr);

	std::shared_ptr<LoxFunction> method = superclass->findMethod(expr->m_method->lexeme);
	if (!method)
	{
		throw RuntimeError(expr->m_method,
			L"Undefined property '" + expr->m_method->lexeme + L"'.");
	}
	return method->bind(obj);
}

std::any
Interpreter::visitThisExpr(std::shared_ptr<This> expr)
{
	return lookUpVariable(expr->m_keyword, expr);
}

std::any
Interpreter::visitUnaryExpr(std::shared_ptr<Unary> expr)
{
	std::shared_ptr<Expr> right = evaluate(expr->m_right);
	if (expr->m_operatorX->type == MINUS)
	{
		checkNumberOperand(expr->m_operatorX, right);
		auto ptr = std::dynamic_pointer_cast<DoubleLiteral>(right);
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
	return lookUpVariable(expr->m_name, expr);
}

std::any
Interpreter::lookUpVariable(std::shared_ptr<Token> name, std::shared_ptr<Expr> expr)
{
	auto it = locals.find(expr);
	if (it != locals.end())
	{
		int distance = it->second;
		return environment->getAt(distance, name->lexeme);
	}
	else
	{
		return globals->get(name);
	}
}

std::any
Interpreter::visitFunctionStmt(std::shared_ptr<Function> stmt)
{
	std::shared_ptr<LoxFunction> func = std::make_shared<LoxFunction>(stmt, environment, false);
	environment->define(stmt->m_name->lexeme, func);
	return std::make_shared<NilLiteral>();
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
	std::shared_ptr<Expr> value = evaluate(stmt->m_expression);
	std::wcout << stringify(value) << std::endl;
	return value;
}

std::any
Interpreter::visitReturnStmt(std::shared_ptr<Return> stmt)
{
	std::shared_ptr<Expr> value;
	if (stmt->m_value)
	{
		value = evaluate(stmt->m_value);
	}
	throw ReturnError(value);
}

std::shared_ptr<Expr>
Interpreter::castToExpr(std::any value)
{
	try
	{
		// TODO: find a way to do this without trying different data types
		std::shared_ptr<StringLiteral> s = std::any_cast<std::shared_ptr<StringLiteral>>(value);
		return s;
	}
	catch (const std::bad_any_cast &e)
	{
		try
		{
			std::shared_ptr<DoubleLiteral> d = std::any_cast<std::shared_ptr<DoubleLiteral>>(value);
			return d;
		}
		catch (const std::bad_any_cast &e)
		{
			try
			{
				std::shared_ptr<BooleanLiteral> b = std::any_cast<std::shared_ptr<BooleanLiteral>>(value);
				return b;
			}
			catch (const std::bad_any_cast &e)
			{
				try
				{
					std::shared_ptr<NilLiteral> n = std::any_cast<std::shared_ptr<NilLiteral>>(value);
					return n;
				}
				catch (const std::bad_any_cast &e)
				{
					std::shared_ptr<Expr> expr = std::any_cast<std::shared_ptr<Expr>>(value);
					return expr;
				}
			}
		}
	}
	return nullptr;
}

std::any
Interpreter::visitVarStmt(std::shared_ptr<Var> stmt)
{
	if (stmt->m_initializer)
	{
		auto expr = evaluate(stmt->m_initializer);
		environment->define(stmt->m_name->lexeme, expr);
	}
	else
	{
		environment->define(stmt->m_name->lexeme, std::make_shared<NilLiteral>());
	}
	return std::make_shared<NilLiteral>();
}

std::any
Interpreter::visitWhileStmt(std::shared_ptr<While> stmt)
{
	while (isTruthy(evaluate(stmt->m_condition)))
	{
		execute(stmt->m_body);
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
Interpreter::checkNumberOperand(std::shared_ptr<Token> operatorX, std::shared_ptr<Expr> operand)
{
	try
	{
		std::shared_ptr<DoubleLiteral> d = std::dynamic_pointer_cast<DoubleLiteral>(operand);
		if (d)
		{
			return;
		}
	}
	catch (const std::bad_any_cast &e)
	{
	}
	throw RuntimeError(operatorX, L"Operand must be a number.");
}

void
Interpreter::checkNumberOperands(std::shared_ptr<Token> operatorX,
	std::shared_ptr<Expr> left,
	std::shared_ptr<Expr> right)
{
	try
	{
		std::dynamic_pointer_cast<DoubleLiteral>(left);
		std::dynamic_pointer_cast<DoubleLiteral>(right);
		if (left && right)
		{
			return;
		}
	}
	catch (const std::bad_any_cast &e)
	{
	}
	throw RuntimeError(operatorX, L"Operands must be numbers.");
}

bool
Interpreter::isTruthy(std::shared_ptr<Expr> n)
{
	try
	{
		auto nilValue = std::dynamic_pointer_cast<NilLiteral>(n);
		if (nilValue)
		{
			return false;
		}
		auto booleanValue = std::dynamic_pointer_cast<BooleanLiteral>(n);
		if (booleanValue)
		{
			return booleanValue->m_value;
		}
	}
	catch (const std::bad_any_cast &e)
	{
	}
	return true;
}

bool
Interpreter::isEqual(std::shared_ptr<Expr> a, std::shared_ptr<Expr> b)
{
	try
	{
		std::shared_ptr<NilLiteral> na = std::dynamic_pointer_cast<NilLiteral>(a);
		std::shared_ptr<NilLiteral> nb = std::dynamic_pointer_cast<NilLiteral>(b);
		if (na && nb)
		{
			return true;
		}
		else if (na)
		{
			return false;
		}

		std::shared_ptr<DoubleLiteral> da = std::dynamic_pointer_cast<DoubleLiteral>(a);
		std::shared_ptr<DoubleLiteral> db = std::dynamic_pointer_cast<DoubleLiteral>(b);
		if (da && db)
		{
			return (da->m_value == db->m_value);
		}

		std::shared_ptr<StringLiteral> sa = std::dynamic_pointer_cast<StringLiteral>(a);
		std::shared_ptr<StringLiteral> sb = std::dynamic_pointer_cast<StringLiteral>(b);
		if (sa && sb)
		{
			return (sa->m_value == sb->m_value);
		}

		std::shared_ptr<BooleanLiteral> ba = std::dynamic_pointer_cast<BooleanLiteral>(a);
		std::shared_ptr<BooleanLiteral> bb = std::dynamic_pointer_cast<BooleanLiteral>(b);
		if (ba && bb)
		{
			return (ba->m_value == bb->m_value);
		}
	}
	catch (const std::bad_any_cast &e)
	{
	}
	return false;
}

std::shared_ptr<Expr>
Interpreter::evaluate(std::shared_ptr<Expr> expr)
{
	std::any result = expr->accept(this);
	return castToExpr(result);
}

std::wstring
Interpreter::stringify(std::shared_ptr<Expr> n)
{
	try
	{
		std::shared_ptr<NilLiteral> n2 = std::dynamic_pointer_cast<NilLiteral>(n);
		if (n2)
		{
			return std::wstring(L"nil");
		}

		std::shared_ptr<StringLiteral> s = std::dynamic_pointer_cast<StringLiteral>(n);
		if (s)
		{
			return s->m_value;
		}

		std::shared_ptr<BooleanLiteral> b = std::dynamic_pointer_cast<BooleanLiteral>(n);
		if (b)
		{
			return b->m_value ? std::wstring(L"true") : std::wstring(L"false");
		}

		std::shared_ptr<DoubleLiteral> d = std::dynamic_pointer_cast<DoubleLiteral>(n);
		if (d)
		{
			std::wostringstream os;
			os.precision(15);
			os << d->m_value;
			return os.str();
		}

		std::shared_ptr<LoxFunction> f = std::dynamic_pointer_cast<LoxFunction>(n);
		if (f)
		{
			return f->toString();
		}

		std::shared_ptr<LoxClass> c = std::dynamic_pointer_cast<LoxClass>(n);
		if (c)
		{
			return c->toString();
		}

		std::shared_ptr<LoxInstance> i = std::dynamic_pointer_cast<LoxInstance>(n);
		if (i)
		{
			return i->toString();
		}
	}
	catch (const std::bad_any_cast &e)
	{
	}
	return std::wstring(L"");
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

void
Interpreter::resolve(std::shared_ptr<Expr> expr, int depth)
{
	locals.insert_or_assign(expr, depth);
}
