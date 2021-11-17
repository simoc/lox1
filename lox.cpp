#include <iostream>
#include <fstream>
#include <string>

#include "lox.h"
#include "scanner.h"
#include "parser.h"
#include "expr.h"

bool Lox::hadError = false;
bool Lox::hadRuntimeError = false;
Interpreter Lox::interpreter;

void
Lox::report(int line, const std::wstring &where, const std::wstring &message)
{
	std::wcerr << "[line " << line << "] Error" <<
		where << ": " << message << std::endl;
	hadError = true;
}

void
Lox::error(int line, const std::wstring &message)
{
	report(line, std::wstring(), message);
}

void
Lox::error(Token token, const std::wstring &message)
{
	if (token.type == END_OF_FILE)
	{
		report(token.line, L" at end", message);
	}
	else
	{
		report(token.line, L" at '" + token.lexeme + L"'", message);
	}
}

void
Lox::runtimeError(const RuntimeError &error)
{
	std::wcerr << error.what() << std::endl;
	std::wcerr << "[line " << error.token->line << "]" << std::endl;
	hadRuntimeError = true;
}

void
Lox::run(const std::wstring &bytes)
{
	Scanner scanner(bytes);
	auto tokens = scanner.scanTokens();
	Parser parser(tokens);
	std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

	// Stop if there was a syntax error.
	if (hadError)
	{
		return;
	}
	interpreter.interpret(statements);
}

void
Lox::runFile(char *path)
{
	std::wifstream f(path);
	if (f)
	{
		std::wstring bytes;
		const int bufSize = 512;
		wchar_t buf[bufSize];
		std::streamsize gcount = 0;
		do
		{
			f.read(buf, bufSize);
			gcount = f.gcount();
			if (gcount > 0)
			{
				bytes.insert(bytes.end(), buf, buf + gcount);
			}
		}
		while (gcount > 0);

		run(bytes);

		if (hadError)
		{
			exit(65);
		}
		if (hadRuntimeError)
		{
			exit(70);
		}
	}
}

void
Lox::runPrompt()
{
	while(true)
	{
		std::wstring line;
		std::wcout << L"> ";
		std::getline(std::wcin, line);
		if (std::wcin.eof())
		{
			break;
		}
		run(line);

		hadError = false;
	}
}
