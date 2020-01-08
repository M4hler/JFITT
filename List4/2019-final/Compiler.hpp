#include <iostream>
using namespace std;

class Compiler
{
public:
	Compiler(FILE *);
	void assign(string, long long);
	void generateConstant(long long);
	void read(string);
	void write();

	void endProgram();
	void setIdentifier(string);
	void setIndexIdentifier(string);
	void setNumberIdentifier(long long);
	void setIdentifierFlag();
	void setValueNumber();
	void placeToStore(string);
	void clear();
	void evaluateExpression();
	void setLeftSideOption(int);
	void copy();
	void add();
	void sub();
	void mul();
	void div();
	void mod();

private:
	FILE * output;
	string identifier;
	string indexIdentifier;
	long long numberIdentifier;
	bool identifierFlag;
	long long storePlace;
	bool valueNumber;
	int leftSideOption;

	void writeToFile(string);
};
