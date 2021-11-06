generateast: generateast.cpp
	gcc -g -std=c++17 -o $@ $^ -lstdc++

expr.h: generateast
	./generateast .

astprinter.cpp: expr.h

astprinter: astprinter.cpp token.cpp
	gcc -g -std=c++17 -o $@ $^ -lstdc++

lox1: main.cpp lox.cpp scanner.cpp token.cpp
	gcc -g -std=c++17 -o $@ $^ -lstdc++

all: generateast astprinter lox1

clean:
	rm -f generateast astprinter lox1 expr.h
