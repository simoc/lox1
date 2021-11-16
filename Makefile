CXXFLAGS=-Wall -g -std=c++17

generateast: generateast.cpp
	gcc $(CXXFLAGS) -o $@ $^ -lstdc++

expr.h: generateast
	./generateast .

astprinter.cpp: expr.h

astprintermain: astprintermain.cpp astprinter.cpp token.cpp
	gcc $(CXXFLAGS) -o $@ $^ -lstdc++

lox1: main.cpp lox.cpp scanner.cpp token.cpp parser.cpp interpreter.cpp
	gcc $(CXXFLAGS) -o $@ $^ -lstdc++

all: generateast astprintermain lox1

clean:
	rm -f generateast astprintermain lox1 expr.h
