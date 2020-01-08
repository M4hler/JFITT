#all: comp

#grammar.tab.cpp grammar.tab.hpp:	grammar.ypp
#	bison -d grammar.ypp

#lexer.lex.cpp:	lexer.l	grammar.tab.hpp
#	flex -o lexer.lex.cpp lexer.l

#comp: lexer.lex.cpp grammar.tab.cpp grammar.tab.hpp Main.cpp SymbolsTable.cpp SymbolsTable.hpp Compiler.cpp Compiler.hpp
#	g++ -o comp grammar.tab.cpp lexer.lex.cpp SymbolsTable.cpp Compiler.cpp Main.cpp

all: comp

bison:
	bison -o grammar.cpp -d grammar.ypp

flex:
	flex -o lexer.cpp lexer.l

parser: flex bison
	g++ -c lexer.cpp -o lexer.o
	g++ -c grammar.cpp -o grammar.o

comp: parser
	g++ -o comp lexer.o grammar.o UninitializedVariable.cpp UndeclaredVariable.cpp BadIndex.cpp SymbolsTable.cpp Compiler.cpp Main.cpp
