#include <iostream>
#include <string>

#include "lox.h"

int
main(int argc, char *argv[])
{
	// Assume UTF-8 for stdin, stdout, stderr.
	// Anything more complex is difficult to handle.
	std::ios_base::sync_with_stdio(false);
	std::wcin.imbue(std::locale("C.UTF-8"));
	std::wcout.imbue(std::locale("C.UTF-8"));
	std::wcerr.imbue(std::locale("C.UTF-8"));

	Lox lox;
	if (argc > 2)
	{
		std::wcerr << "Usage: " << argv[0] << " [script]" << std::endl;
		exit(64);
	}
	else if (argc == 2)
	{
		lox.runFile(argv[1]);
	}
	else
	{
		lox.runPrompt();
	}
}
