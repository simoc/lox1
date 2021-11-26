#pragma once

#include <memory>
#include <stdexcept>

#include "expr.h"

/**
 * Exception for return keyword from 10.5.1.
 */
class ReturnError : public std::runtime_error
{
public:
	ReturnError(std::shared_ptr<Expr> _value) :
		std::runtime_error(std::string()),
		value(_value)
	{
	};

	std::shared_ptr<Expr> value;
};
