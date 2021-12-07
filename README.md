# lox1

Lox scripting language interpreter from Chapters 1-13 of the http://www.craftinginterpreters.com web site and book.

This implementation is in C++ instead of Java, and the same class structure, method names and source code generation are used.

However, the widespread use of `java.lang.Object` to hold basic data types cannot be directly mapped to C++,
and the additional C++ classes `StringLiteral`, `DoubleLiteral`, `BooleanLiteral` and `NilLiteral`
have been created.

## Compiling

Build the interpreter using the `Makefile` and `gcc` with the command:

    make all
