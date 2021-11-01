lox1: main.cpp lox.cpp scanner.cpp token.cpp
	gcc -std=c++11 -o $@ $^ -lstdc++

clean:
	rm -f lox1
