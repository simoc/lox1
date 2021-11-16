#pragma once

#include <stdexcept>
#include <string>
#include <memory>

#include "token.h"

class RuntimeError : public std::runtime_error
{
public:
	RuntimeError(std::shared_ptr<Token> _token, const std::wstring &message) :
		std::runtime_error(std::string(message.begin(), message.end())),
		token(_token)
	{
	};

	std::shared_ptr<Token> token;
};
