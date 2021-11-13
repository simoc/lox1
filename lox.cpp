#include <iostream>
#include <fstream>
#include <string>

#include "lox.h"
#include "scanner.h"
#include "parser.h"
#include "expr.h"
#include "astprinter.h"

bool Lox::hadError = false;

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
Lox::run(const std::wstring &bytes)
{
	Scanner scanner(bytes);
	auto tokens = scanner.scanTokens();
	Parser<std::wstring> parser(tokens);
	std::shared_ptr<Expr<std::wstring>> expr = parser.parse();

	// Stop if there was a syntax error.
	if (hadError)
	{
		return;
	}
	AstPrinter printer;
	std::wcout << printer.print(expr) << std::endl;
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
