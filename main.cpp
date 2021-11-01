#include <iostream>
#include <string>

#include "lox.h"

int
main(int argc, char *argv[])
{
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
