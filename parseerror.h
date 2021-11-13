#pragma once

#include <stdexcept>

class ParseError : std::runtime_error
{
public:
	ParseError(const std::wstring &message) :
		std::runtime_error(std::string(message.begin(), message.end()))
	{
	}
};
