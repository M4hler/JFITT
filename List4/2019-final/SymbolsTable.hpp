#include <vector>
#include <string>
using namespace std;

class SymbolsTable
{
public:
	SymbolsTable();
	void varDeclaration(string);
	void varDeclaration(string, long long, long long);
	void checkIfDeclared(string);
	//void checkIfDeclared(string, long long);
	void showTable();
	long long getVarMemoryLocation(string);
	void checkIfInitialized(string);
	bool checkIfArray(string);
	void checkIfIndexIsInRange(string, long long);
	void initVariable(string);
	long long getFreeMemoryCell();
	long long getArrayFirstIndex(string);
	void deleteVar(string);

private:
	vector<string> * varName;
	vector<bool> * initialised;
	//vector<long long> * value;
	vector<long long> * memoryLocationStart;
	vector<long long> * memoryLocationEnd;
	vector<bool> * isArray;
	vector<long long> * firstIndex;
	vector<long long> * lastIndex;
	long long firstFreeMemoryCell;

	void checkIfAlreadyDeclared(string);
	void checkStartLessThanEnd(string, long long, long long);
	void declare(string, long long, long long, bool);
};
