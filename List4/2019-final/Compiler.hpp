#include <iostream>
#include <vector>
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

	void printNest();

	void forAsc(string);
	void forDesc(string);
	void endFor(string);
	void endForDesc(string);

	void endWhile();
	void setWhileFlag();
	void whileJump();

	void addJump();
	void condEq();
	void condNeq();
	void condLe();
	void condGe();
	void condLeq();
	void condGeq();
	void endIf();
	void endElse();
	void replaceJumpLocations(FILE *, FILE *);

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
	vector<long long> * jumpLocations;
	vector<long long> * nestedVector;

	vector<long long> * jumpForLocations;
	vector<long long> * nestedFor;

	vector<long long> * jumpForBackLocations;
	vector<long long> * nestedForBack;

	vector<long long> * jumpWhileLocations;
	vector<long long> * nestedWhile;

	vector<long long> * jumpWhileBackLocations;
	vector<long long> * nestedWhileBack;

	bool inWhileFlag;

	void writeToFile(string);
};
