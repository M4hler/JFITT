#include <iostream>
#include <string>
#include "SymbolsTable.hpp"

#include "AlreadyDeclared.hpp"
#include "WrongIndexes.hpp"
#include "UndeclaredVariable.hpp"
#include "UninitializedVariable.hpp"
#include "BadIndex.hpp"

#include "Compiler.hpp"
using namespace std;

extern long long lineNumber;
extern long long generatedLines;
extern Compiler * compiler;

//public
SymbolsTable::SymbolsTable()
{
	varName = new vector<string>();
	initialised = new vector<bool>();
	//value = new vector<long long>();
	memoryLocationStart = new vector<long long>();
	memoryLocationEnd = new vector<long long>();
	isArray = new vector<bool>();
	firstIndex = new vector<long long>();
	lastIndex = new vector<long long>();
	firstFreeMemoryCell = 1;
}

void SymbolsTable::varDeclaration(string varIdentifier)
{
	try
	{
		checkIfAlreadyDeclared(varIdentifier);

		declare(varIdentifier, 0, 0, false);
	}
	catch(AlreadyDeclared alreadyDeclared)
	{
		cout << alreadyDeclared.message << endl;
	}
}

void SymbolsTable::varDeclaration(string varIdentifier, long long rangeStart, long long rangeEnd)
{
	try
	{
		checkIfAlreadyDeclared(varIdentifier);

		checkStartLessThanEnd(varIdentifier, rangeStart, rangeEnd);

		declare(varIdentifier, rangeStart, rangeEnd, true);
		firstFreeMemoryCell--;

		for(int i = rangeStart; i <= rangeEnd; i++)
		{
			declare(varIdentifier + "(" + to_string(i) + ")", 0, 0, false);
			/*if(i % 1000000 == 0)
			{
				cout << varIdentifier + "(" + to_string(i) + ")" << endl;
			}*/
		}
	}
	catch(AlreadyDeclared alreadyDeclared)
	{
		cout << alreadyDeclared.message << endl;
	}
	catch(WrongIndexes wrongIndexes)
	{
		cout << wrongIndexes.message << endl;
	}
}

void SymbolsTable::checkIfDeclared(string varIdentifier)
{
	for(int i = 0; i < varName -> size(); i++)
	{
		if(varIdentifier == varName -> at(i))
		{
			return;
		}
	}

	throw UndeclaredVariable(varIdentifier);
}

void SymbolsTable::showTable()
{
	//cout << "Name\tInit\tValue\tMemSt\tMemEnd\tIsArray\tFstInd\tLastInd" << endl;
	cout << "Name\tInit\tMemSt\tMemEnd\tIsArray\tFstInd\tLastInd" << endl;

	for(int i = 0; i < varName -> size(); i++)
	{
			cout << varName -> at(i) << " " 
			<< "\t" << initialised -> at(i) << " "
			//<< "\t" << value -> at(i) << " "
			<< "\t" << memoryLocationStart -> at(i) << " " 
			<< "\t" << memoryLocationEnd -> at(i) << " "
			<< "\t" << isArray -> at(i) << " "
			<< "\t" << firstIndex -> at(i) << " "
			<< "\t" << lastIndex -> at(i) << " " << endl;
	}
	cout << "First free memory cell: " << firstFreeMemoryCell << endl;
	cout << "Generated lines: " << generatedLines << endl;
}

long long SymbolsTable::getVarMemoryLocation(string varIdentifier)
{
	for(int i = 0; i < varName -> size(); i++)
	{
		if(varIdentifier == varName -> at(i))
		{
			return memoryLocationStart -> at(i);
		}
	}
}

void SymbolsTable::checkIfInitialized(string varIdentifier)
{
	for(int i = 0; i < varName -> size(); i++)
	{
		if(varIdentifier == varName -> at(i))
		{
			if(initialised -> at(i) == false)
			{
				throw UninitializedVariable(varIdentifier);
			}

			return;
		}
	}
}

bool SymbolsTable::checkIfArray(string varIdentifier)
{
	for(int i = 0; i < varName -> size(); i++)
	{
		if(varIdentifier == varName -> at(i))
		{
			if(isArray -> at(i) == true)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

void SymbolsTable::checkIfIndexIsInRange(string varIdentifier, long long index)
{
	int j = 0;
	for(int i = 0; i < varName -> size(); i++)
	{
		if(varIdentifier == varName -> at(i))
		{
			j = i;
			if(index >= firstIndex -> at(i) && index <= lastIndex -> at(i))
			{
				return;
			}
		}
	}

	throw BadIndex(varIdentifier, index, firstIndex -> at(j), lastIndex -> at(j));
}

void SymbolsTable::initVariable(string varIdentifier)
{
	for(int i = 0; i < varName -> size(); i++)
	{
		if(varIdentifier == varName -> at(i))
		{
			initialised -> at(i) = true;
		}
	}
}

long long SymbolsTable::getFreeMemoryCell()
{
	return firstFreeMemoryCell;
}

long long SymbolsTable::getArrayFirstIndex(string varIdentifier)
{
	for(int i = 0; i < varName -> size(); i++)
	{
		if(varIdentifier == varName -> at(i))
		{
			return firstIndex -> at(i);
		}
	}
}

//private
void SymbolsTable::checkIfAlreadyDeclared(string varIdentifier)
{
	for(int i = 0; i < varName -> size(); i++)
	{
		if(varIdentifier == varName -> at(i))
		{
			throw AlreadyDeclared(varIdentifier, lineNumber);
		}
	}
}

void SymbolsTable::checkStartLessThanEnd(string varIdentifier, long long start, long long end)
{
	if(start > end)
	{
		throw WrongIndexes(varIdentifier, start, end, lineNumber);
	}
}

void SymbolsTable::declare(string varIdentifier, long long rangeStart, long long rangeEnd, bool amIArray)
{
	varName -> push_back(varIdentifier);
	initialised -> push_back(false); //variable hasn't been asigned
	//value -> push_back(0); //default value
	memoryLocationStart -> push_back(firstFreeMemoryCell);
	memoryLocationEnd -> push_back(firstFreeMemoryCell + rangeEnd - rangeStart);
	isArray -> push_back(amIArray);
	firstIndex -> push_back(rangeStart);
	lastIndex -> push_back(rangeEnd);
	firstFreeMemoryCell++;
}
