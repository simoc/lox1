#include <iostream>
#include <fstream>
#include <string>

#include "lox.h"
#include "scanner.h"

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
Lox::run(const std::wstring &bytes)
{
	Scanner scanner(bytes);
	auto tokens = scanner.scanTokens();
	for (const auto &token : tokens)
	{
		std::wcout << token << std::endl;
	}
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
		std::cout << "> ";
		std::getline(std::wcin, line);
		if (std::wcin.eof())
		{
			break;
		}
		run(line);

		hadError = false;
	}
}
