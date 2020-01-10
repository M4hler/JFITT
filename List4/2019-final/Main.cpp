#include <iostream>
#include <cstring>
#include <fstream>
#include "SymbolsTable.hpp"
#include "Compiler.hpp"
#include "grammar.hpp"
using namespace std;

extern FILE * yyin;
SymbolsTable * symbolsTable;
Compiler * compiler;

int main(int argc, char ** argv)
{
	FILE * outputFile = new FILE();

	yyin = fopen(argv[1], "r");

	if(argc > 1)
	{
		outputFile = fopen(argv[2], "w");
	}
	
	symbolsTable = new SymbolsTable();
	compiler = new Compiler(outputFile);

	while(!feof(yyin))
	{
		yyparse();
	}

	//symbolsTable -> showTable();
}
