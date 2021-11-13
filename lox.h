#pragma once

#include "token.h"

class Lox
{
public:
	void runPrompt();

	void runFile(char *path);

	static void error(int line, const std::wstring &message);

	static void error(Token token, const std::wstring &message);
private:
	static bool hadError;

	static void report(int line, const std::wstring &where,
		const std::wstring &message);


	void run(const std::wstring &bytes);

};
