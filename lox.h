#pragma once

#include <memory>

#include "token.h"
#include "interpreter.h"
#include "runtimeerror.h"

class Lox
{
public:
	void runPrompt();

	void runFile(char *path);

	static void error(int line, const std::wstring &message);

	static void error(std::shared_ptr<Token> token, const std::wstring &message);

	static void runtimeError(const RuntimeError &error);
private:
	static bool hadError;
	static bool hadRuntimeError;
	static Interpreter interpreter;

	static void report(int line, const std::wstring &where,
		const std::wstring &message);


	void run(const std::wstring &bytes);

};
