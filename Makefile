lox1: main.cpp lox.cpp scanner.cpp token.cpp
	gcc -g -std=c++11 -o $@ $^ -lstdc++

all: lox1

clean:
	rm -f lox1
