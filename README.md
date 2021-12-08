![main workflow](https://github.com/simoc/lox1/actions/workflows/main.yml/badge.svg)

# lox1

Lox scripting language interpreter from Chapters 1-13 of the http://www.craftinginterpreters.com web site and book.

This implementation is in C++ instead of Java, and the same class structure, method names and source code generation are used.

However, the widespread use of `java.lang.Object` to hold basic data types cannot be directly mapped to C++,
and the additional C++ classes `StringLiteral`, `DoubleLiteral`, `BooleanLiteral` and `NilLiteral`
have been created.

As garbage collection is not available in C++, `std::shared_ptr` is used to ensure that memory used by the interpreter is freed after use.

## Compiling

Build the interpreter using the `Makefile` and `gcc` with the command:

    make all
    
## Usage

```
$ ./lox1
> print "hello" + "world!";
helloworld!
> fun sum(a, b, c) { return a + b + c; }
> print sum(1, 4, 8);
13
>
```
