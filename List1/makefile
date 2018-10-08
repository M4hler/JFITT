FLAGS= -Wall --pedantic

all: naiveMatcher dfaMatcher main

naiveMatcher: ./NaiveMatcher.h; \
	g++ $< -o $@ $(FLAGS)

dfaMatcher: ./DFAMatcher.h; \
	g++ $< -o $@ $(FLAGS)

main:	./main.cpp; \
	g++ $< -o $@ $(FLAGS)

.clean:	rm -rf .out .o

