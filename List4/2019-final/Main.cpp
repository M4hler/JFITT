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
	FILE * out2 = new FILE();

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

	fclose(outputFile);

	outputFile = fopen(argv[2], "r");
	out2 = fopen("temporaryFile.txt", "w");

	char buff[255];
	while(!feof(outputFile))
	{
		fgets(buff, 255, outputFile);
		string s = string(buff);

		char cString[s.size() + 1];
		strcpy(cString, s.c_str());
		fputs(cString, out2);

		if(s == "HALT\n")
			break;
	}

	fclose(outputFile);
	fclose(out2);

	outputFile = fopen(argv[2], "w");
	out2 = fopen("temporaryFile.txt", "r");

	compiler -> replaceJumpLocations(outputFile, out2);
	
	fclose(outputFile);
	fclose(out2);

	//remove("temporaryFile.txt");
	symbolsTable -> showTable();
	compiler -> printNest();
}
