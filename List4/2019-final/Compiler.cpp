#include <iostream>
#include <cstring>
#include <algorithm>
#include "Compiler.hpp"
#include "SymbolsTable.hpp"

#include "UndeclaredVariable.hpp"
#include "UninitializedVariable.hpp"
#include "BadIndex.hpp"
using namespace std;

extern SymbolsTable * symbolsTable;
extern long long generatedLines;
extern long long flagNumber;
extern long long ifNested;

extern long long forFlagNumber;
extern long long forNested;

extern long long forJumpBackFlag;
extern long long forJumpBackNested;

extern long long whileFlagNumber;
extern long long whileNested;

//public
Compiler::Compiler(FILE * file)
{
	output = file;
	identifier = "";
	indexIdentifier = "";
	numberIdentifier = 0;
	identifierFlag = false;
	valueNumber = false;
	leftSideOption = 0;
	jumpLocations = new vector<long long>();
	nestedVector = new vector<long long>();

	jumpForLocations = new vector<long long>();
	nestedFor = new vector<long long>();

	jumpForBackLocations = new vector<long long>();
	nestedForBack = new vector<long long>();

	jumpWhileLocations = new vector<long long>();
	nestedWhile = new vector<long long>();

	jumpWhileBackLocations = new vector<long long>();
	nestedWhileBack = new vector<long long>();

	inWhileFlag = false;
}

void Compiler::assign(string varIdentifier, long long value)
{
	try
	{
		symbolsTable -> checkIfDeclared(varIdentifier);

		if(leftSideOption == 2)
		{
			string expr = "STOREI " + to_string(storePlace) + "\n";

			indexIdentifier = "";
			writeToFile(expr);
		}
		else if(leftSideOption == 1)
		{
			string fullName = identifier + "(" + to_string(numberIdentifier) + ")";
			symbolsTable -> initVariable(fullName);

			string expr = "STORE " + to_string(storePlace) + "\n";

			writeToFile(expr);
		}
		else
		{
			symbolsTable -> initVariable(varIdentifier);

			string expr = "STORE " + to_string(storePlace) + "\n";

			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
}

void Compiler::generateConstant(long long constant)
{
	string expr = "";

	if(constant == 0)
	{
		expr = "SUB 0\n" + expr;
	}
	else if(constant < 0)
	{
		while(constant < -1)
		{
			if(constant % 2 == -1)
			{
				expr = "DEC\n" + expr;
			}

			expr = "ADD 0\n" + expr;

			constant /= 2;
		}

		expr = "SUB 0\nDEC\n" + expr;
	}
	else
	{
		while(constant > 1)
		{
			if(constant % 2 == 1)
			{
				expr = "INC\n" + expr;
			}

			expr = "ADD 0\n" + expr;

			constant /= 2;
		}

		expr = "SUB 0\nINC\n" + expr;
	}

	writeToFile(expr);
}

void Compiler::read(string varIdentifier)
{
	try
	{
		symbolsTable -> checkIfDeclared(varIdentifier);
		
		if(symbolsTable -> checkIfArray(varIdentifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				//generate array start memory location
				generateConstant(symbolsTable -> getVarMemoryLocation(varIdentifier));
				long long freeCell = symbolsTable -> getFreeMemoryCell();				
				string expr = "STORE " + to_string(freeCell) + "\n";
				writeToFile(expr);

				//generate first available index
				expr = "";
				long long index = symbolsTable -> getArrayFirstIndex(varIdentifier);
				generateConstant(index);
				expr += "STORE " + to_string(freeCell + 1) + "\n";
								
				//load value of indexIdentifier
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(indexIdentifier);
				expr += "LOAD " + to_string(memoryLocation) + "\n";

				//calculate difference between indetifier value and first available index
				expr += "SUB " + to_string(freeCell + 1) + "\n";

				//add array start memory location
				expr += "ADD " + to_string(freeCell) + "\n";

				//store it
				expr += "STORE " + to_string(freeCell + 2) + "\n";

				//get external value
				expr += "GET\n";

				//storei in proper place 
				expr += "STOREI " + to_string(freeCell + 2) + "\n";

				indexIdentifier = "";

				writeToFile(expr);
			}
			else //array(number), number is stored in numberIdentifier
			{
				symbolsTable -> checkIfIndexIsInRange(varIdentifier, numberIdentifier);
				
				string fullName = varIdentifier + "(" + to_string(numberIdentifier) + ")";
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(fullName);
				symbolsTable -> initVariable(fullName);

				string expr = "GET\n";
				expr += "STORE " + to_string(memoryLocation) + "\n";

				writeToFile(expr);
			}
		}
		else
		{
			long long memoryLocation = symbolsTable -> getVarMemoryLocation(varIdentifier);
			symbolsTable -> initVariable(varIdentifier);

			string expr = "GET\n";
			expr += "STORE " + to_string(memoryLocation) + "\n";

			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

void Compiler::write()
{
	try
	{
		string expr = "";

		if(identifierFlag == true)
		{
			if(symbolsTable -> checkIfArray(identifier) == true)
			{
				if(indexIdentifier != "")
				{
					symbolsTable -> checkIfDeclared(indexIdentifier);
					symbolsTable -> checkIfInitialized(indexIdentifier);

					//generate array start memory location
					generateConstant(symbolsTable -> getVarMemoryLocation(identifier));
					long long freeCell = symbolsTable -> getFreeMemoryCell();				
					expr += "STORE " + to_string(freeCell) + "\n";
					writeToFile(expr);

					//generate first available index
					expr = "";
					long long index = symbolsTable -> getArrayFirstIndex(identifier);
					generateConstant(index);
					expr += "STORE " + to_string(freeCell + 1) + "\n";

					//load value of indexIdentifier
					long long memoryLocation = symbolsTable -> getVarMemoryLocation(indexIdentifier);
					expr += "LOAD " + to_string(memoryLocation) + "\n";

					//calculate difference between indetifier value and first available index
					expr += "SUB " + to_string(freeCell + 1) + "\n";

					//add array start memory location
					expr += "ADD " + to_string(freeCell) + "\n";

					//store it
					expr += "STORE " + to_string(freeCell + 2) + "\n";

					//load it
					expr += "LOADI " + to_string(freeCell + 2) + "\n";

					indexIdentifier = "";
				}
				else //array(number)
				{
					symbolsTable -> checkIfIndexIsInRange(identifier, numberIdentifier);

					string fullName = identifier + "(" + to_string(numberIdentifier) + ")";
					long long memoryLocation = symbolsTable -> getVarMemoryLocation(fullName);

					expr = "LOAD " + to_string(memoryLocation) + "\n";
				}
			}
			else
			{
				symbolsTable -> checkIfDeclared(identifier);
				symbolsTable -> checkIfInitialized(identifier);

				long long memoryLocation = symbolsTable -> getVarMemoryLocation(identifier);
				expr += "LOAD " + to_string(memoryLocation) + "\n";

				identifier = "";
			}				
			expr += "PUT\n";

			writeToFile(expr);

			identifierFlag = false;
		}
		else //generate
		{
			expr = "PUT\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

void Compiler::endProgram()
{
	string halt = "HALT\n";

	writeToFile(halt);
}

void Compiler::setIndexIdentifier(string id)
{
	indexIdentifier = id;
}

void Compiler::setNumberIdentifier(long long num)
{
	numberIdentifier = num;
}

void Compiler::setIdentifier(string varIdentifier)
{
	identifier = varIdentifier;
}

void Compiler::setIdentifierFlag()
{
	identifierFlag = true;
}

void Compiler::setValueNumber()
{
	valueNumber = true;
}

void Compiler::clear()
{
	identifier = "";
	indexIdentifier = "";
	identifierFlag = false;
	valueNumber = false;
}

void Compiler::placeToStore(string varIdentifier)
{
	try
	{
		symbolsTable -> checkIfDeclared(varIdentifier);

		if(symbolsTable -> checkIfArray(varIdentifier) == true)
		{
			if(indexIdentifier != "")
			{
				string expr = "";

				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				//generate array start memory location
				generateConstant(symbolsTable -> getVarMemoryLocation(varIdentifier));
				long long freeCell = symbolsTable -> getFreeMemoryCell();				
				expr += "STORE " + to_string(freeCell) + "\n";
				writeToFile(expr);

				//generate first available index
				expr = "";
				long long index = symbolsTable -> getArrayFirstIndex(varIdentifier);
				generateConstant(index);
				expr += "STORE " + to_string(freeCell + 1) + "\n";
								
				//load value of indexIdentifier
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(indexIdentifier);
				expr += "LOAD " + to_string(memoryLocation) + "\n";

				//calculate difference between indetifier value and first available index
				expr += "SUB " + to_string(freeCell + 1) + "\n";

				//add array start memory location
				expr += "ADD " + to_string(freeCell) + "\n";

				//store it
				expr += "STORE " + to_string(freeCell + 2) + "\n";

				//expr += "STOREI " + to_string(freeCell + 3) + "\n#finito\n";
				//storePlace = freeCell + 3;

				storePlace = freeCell + 2;

				writeToFile(expr);
				//indexIdentifier = ""; //still needed in assign, cleared there
				setLeftSideOption(2);
			}
			else //aray(number)
			{
				symbolsTable -> checkIfIndexIsInRange(varIdentifier, numberIdentifier);

				string fullName = varIdentifier + "(" + to_string(numberIdentifier) + ")";
				storePlace = symbolsTable -> getVarMemoryLocation(fullName);

				setLeftSideOption(1);
			}
		}
		else
		{
			storePlace = symbolsTable -> getVarMemoryLocation(varIdentifier);
			setLeftSideOption(0);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

void Compiler::evaluateExpression()
{
	if(valueNumber == true)
	{
		return;
	}

	try
	{
		symbolsTable -> checkIfDeclared(identifier);

		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				string expr = "";

				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				//generate array start memory location
				generateConstant(symbolsTable -> getVarMemoryLocation(identifier));
				long long freeCell = symbolsTable -> getFreeMemoryCell();				
				expr += "STORE " + to_string(freeCell) + "\n";
				writeToFile(expr);

				//generate first available index
				expr = "";
				long long index = symbolsTable -> getArrayFirstIndex(identifier);
				generateConstant(index);
				expr += "STORE " + to_string(freeCell + 1) + "\n";
								
				//load value of indexIdentifier
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(indexIdentifier);
				expr += "LOAD " + to_string(memoryLocation) + "\n";

				//calculate difference between indetifier value and first available index
				expr += "SUB " + to_string(freeCell + 1) + "\n";

				//add array start memory location
				expr += "ADD " + to_string(freeCell) + "\n";

				expr += "LOADI 0\n";
				writeToFile(expr);
			}
			else //aray(number)
			{
				symbolsTable -> checkIfIndexIsInRange(identifier, numberIdentifier);

				string fullName = identifier + "(" + to_string(numberIdentifier) + ")";
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(fullName);

				string expr = "LOAD " + to_string(memoryLocation) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			long long memoryLocation = symbolsTable -> getVarMemoryLocation(identifier);

			string expr = "LOAD " + to_string(memoryLocation) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

void Compiler::setLeftSideOption(int number)
{
	leftSideOption = number;
}

void Compiler::copy()
{
	string expr = "STORE 1\n";
	writeToFile(expr);
}

void Compiler::endFor(string varIdentifier)
{
	string name = "#iterator" + varIdentifier;
	long long mem = symbolsTable -> getVarMemoryLocation(name);

	long long memoryLocation = symbolsTable -> getVarMemoryLocation(varIdentifier);
	string expr = "LOAD " + to_string(memoryLocation) + "\n";
	expr += "INC\n";
	expr += "STORE " + to_string(memoryLocation) + "\n";
	expr += "JUMP #iteratorJump" + to_string(forJumpBackFlag) + "\n";
	writeToFile(expr);

	nestedFor -> push_back(forNested);
	forNested--;
	jumpForLocations -> push_back(generatedLines);

	nestedForBack -> push_back(forJumpBackNested);
	forJumpBackNested--;
	jumpForBackLocations -> push_back(0);
	forJumpBackFlag++;

	symbolsTable -> deleteVar(name);
	symbolsTable -> deleteVar(varIdentifier);
}

void Compiler::endForDesc(string varIdentifier)
{
	string name = "#iterator" + varIdentifier;
	long long mem = symbolsTable -> getVarMemoryLocation(name);

	long long memoryLocation = symbolsTable -> getVarMemoryLocation(varIdentifier);
	string expr = "LOAD " + to_string(memoryLocation) + "\n";
	expr += "DEC\n";
	expr += "STORE " + to_string(memoryLocation) + "\n";
	expr += "JUMP #iteratorJump" + to_string(forJumpBackFlag) + "\n";
	writeToFile(expr);

	nestedFor -> push_back(forNested);
	forNested--;
	jumpForLocations -> push_back(generatedLines);

	nestedForBack -> push_back(forJumpBackNested);
	forJumpBackNested--;
	jumpForBackLocations -> push_back(0);
	forJumpBackFlag++;

	symbolsTable -> deleteVar(name);
	symbolsTable -> deleteVar(varIdentifier);
}

void Compiler::forDesc(string varIdentifier)
{
	string name = "#iterator" + varIdentifier;
	symbolsTable -> varDeclaration(name);
	symbolsTable -> initVariable(name);

	if(identifier == "")
	{
		long long memoryLocation = symbolsTable -> getVarMemoryLocation(name);
		string expr = "STORE " + to_string(memoryLocation) + "\n";
		writeToFile(expr);

		long long memLoc = symbolsTable -> getVarMemoryLocation(varIdentifier);
		expr = "LOAD 1\n";
		expr += "STORE " + to_string(memLoc) + "\n";
		writeToFile(expr);

		expr = "SUB " + to_string(memoryLocation) + "\n";
		expr += "JNEG #iteratorFlag" + to_string(forFlagNumber) + "\n";

		forFlagNumber++;
		forNested++;
		jumpForLocations -> push_back(0);
		nestedFor -> push_back(forNested);

		forJumpBackNested++;
		jumpForBackLocations -> push_back(generatedLines);
		nestedForBack -> push_back(forJumpBackNested);

		writeToFile(expr);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(name);
				string expr = "STORE " + to_string(memoryLocation) + "\n";
				writeToFile(expr);

				long long memLoc = symbolsTable -> getVarMemoryLocation(varIdentifier);
				expr = "LOAD 1\n";
				expr += "STORE " + to_string(memLoc) + "\n";
				writeToFile(expr);

				expr = "SUB " + to_string(memoryLocation) + "\n";
				expr += "JNEG #iteratorFlag" + to_string(forFlagNumber) + "\n";

				forFlagNumber++;
				forNested++;
				jumpForLocations -> push_back(0);
				nestedFor -> push_back(forNested);

				forJumpBackNested++;
				jumpForBackLocations -> push_back(generatedLines);
				nestedForBack -> push_back(forJumpBackNested);

				writeToFile(expr);
			}
			else
			{
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(name);
				string expr = "STORE " + to_string(memoryLocation) + "\n";
				writeToFile(expr);

				long long memLoc = symbolsTable -> getVarMemoryLocation(varIdentifier);
				expr = "LOAD 1\n";
				expr += "STORE " + to_string(memLoc) + "\n";
				writeToFile(expr);

				expr = "SUB " + to_string(memoryLocation) + "\n";
				expr += "JNEG #iteratorFlag" + to_string(forFlagNumber) + "\n";

				forFlagNumber++;
				forNested++;
				jumpForLocations -> push_back(0);
				nestedFor -> push_back(forNested);

				forJumpBackNested++;
				jumpForBackLocations -> push_back(generatedLines);
				nestedForBack -> push_back(forJumpBackNested);

				writeToFile(expr);
			}
		}
		else
		{
			long long memoryLocation = symbolsTable -> getVarMemoryLocation(name);
			string expr = "STORE " + to_string(memoryLocation) + "\n";
			writeToFile(expr);

			long long memLoc = symbolsTable -> getVarMemoryLocation(varIdentifier);
			expr = "LOAD 1\n";
			expr += "STORE " + to_string(memLoc) + "\n";
			writeToFile(expr);

			expr = "SUB " + to_string(memoryLocation) + "\n";
			expr += "JNEG #iteratorFlag" + to_string(forFlagNumber) + "\n";

			forFlagNumber++;
			forNested++;
			jumpForLocations -> push_back(0);
			nestedFor -> push_back(forNested);

			forJumpBackNested++;
			jumpForBackLocations -> push_back(generatedLines);
			nestedForBack -> push_back(forJumpBackNested);

			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

void Compiler::forAsc(string varIdentifier)
{
	string name = "#iterator" + varIdentifier;
	symbolsTable -> varDeclaration(name);
	symbolsTable -> initVariable(name);

	if(identifier == "")
	{
		long long memoryLocation = symbolsTable -> getVarMemoryLocation(name);
		string expr = "STORE " + to_string(memoryLocation) + "\n";
		writeToFile(expr);

		long long memLoc = symbolsTable -> getVarMemoryLocation(varIdentifier);
		expr = "LOAD 1\n";
		expr += "STORE " + to_string(memLoc) + "\n";
		writeToFile(expr);

		expr = "SUB " + to_string(memoryLocation) + "\n";
		expr += "JPOS #iteratorFlag" + to_string(forFlagNumber) + "\n";

		forFlagNumber++;
		forNested++;
		jumpForLocations -> push_back(0);
		nestedFor -> push_back(forNested);

		forJumpBackNested++;
		jumpForBackLocations -> push_back(generatedLines);
		nestedForBack -> push_back(forJumpBackNested);

		writeToFile(expr);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(name);
				string expr = "STORE " + to_string(memoryLocation) + "\n";
				writeToFile(expr);

				long long memLoc = symbolsTable -> getVarMemoryLocation(varIdentifier);
				expr = "LOAD 1\n";
				expr += "STORE " + to_string(memLoc) + "\n";
				writeToFile(expr);

				expr = "SUB " + to_string(memoryLocation) + "\n";
				expr += "JPOS #iteratorFlag" + to_string(forFlagNumber) + "\n";

				forFlagNumber++;
				forNested++;
				jumpForLocations -> push_back(0);
				nestedFor -> push_back(forNested);

				forJumpBackNested++;
				jumpForBackLocations -> push_back(generatedLines);
				nestedForBack -> push_back(forJumpBackNested);

				writeToFile(expr);
			}
			else
			{
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(name);
				string expr = "STORE " + to_string(memoryLocation) + "\n";
				writeToFile(expr);

				long long memLoc = symbolsTable -> getVarMemoryLocation(varIdentifier);
				expr = "LOAD 1\n";
				expr += "STORE " + to_string(memLoc) + "\n";
				writeToFile(expr);

				expr = "SUB " + to_string(memoryLocation) + "\n";
				expr += "JPOS #iteratorFlag" + to_string(forFlagNumber) + "\n";

				forFlagNumber++;
				forNested++;
				jumpForLocations -> push_back(0);
				nestedFor -> push_back(forNested);

				forJumpBackNested++;
				jumpForBackLocations -> push_back(generatedLines);
				nestedForBack -> push_back(forJumpBackNested);

				writeToFile(expr);
			}
		}
		else
		{
			long long memoryLocation = symbolsTable -> getVarMemoryLocation(name);
			string expr = "STORE " + to_string(memoryLocation) + "\n";
			writeToFile(expr);

			long long memLoc = symbolsTable -> getVarMemoryLocation(varIdentifier);
			expr = "LOAD 1\n";
			expr += "STORE " + to_string(memLoc) + "\n";
			writeToFile(expr);

			expr = "SUB " + to_string(memoryLocation) + "\n";
			expr += "JPOS #iteratorFlag" + to_string(forFlagNumber) + "\n";

			forFlagNumber++;
			forNested++;
			jumpForLocations -> push_back(0);
			nestedFor -> push_back(forNested);

			forJumpBackNested++;
			jumpForBackLocations -> push_back(generatedLines);
			nestedForBack -> push_back(forJumpBackNested);

			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

void Compiler::replaceJumpLocations(FILE * outFile, FILE * file)
{
	char buff[255];
	int j = 0;
	int prev = -1;

	int jIt = 0;
	int prevIt = -1;

	int jJump = 0;
	int prevJump = -1;

	int jWhile = 0;
	int prevWhile = -1;

	reverse(nestedForBack -> begin(), nestedForBack -> end());
	reverse(jumpForBackLocations -> begin(), jumpForBackLocations -> end());

	reverse(nestedWhileBack -> begin(), nestedWhileBack -> end());
	reverse(jumpWhileBackLocations -> begin(), jumpWhileBackLocations -> end());

	while(!feof(file))
	{
		fgets(buff, 255, file);
		string s = string(buff);
		size_t found = s.find("#flag");
		size_t foundIterator = s.find("#iteratorFlag");
		size_t foundJumpIterator = s.find("#iteratorJump");
		size_t foundWhile = s.find("#whileFlag");

		if(found != string::npos)
		{
			string number = s.substr(found + 5, 10);
			int i = stoi(number);
			//s.replace(found, 10, to_string(jumpLocations -> at(i)) + "\n");
			//cout << "J: " << j << endl;
			if(i == prev)
			{
				j--;
			}
			else
			{
				prev = i;
			}

			int val = nestedVector -> at(j);

			for(int k = j + 1; k < nestedVector -> size(); k++)
			{
				if(nestedVector -> at(k) == val && jumpLocations -> at(k) == 0)
				{
					//cout << "break " << j << " " << k << endl;
					
					j++;
					k = j;
					continue;
					//break;
				}

				if(nestedVector -> at(k) < val)
				{					
					j++;
					k = j;
					val = nestedVector -> at(j);
					continue;
				}

				if(nestedVector -> at(k) == val && jumpLocations -> at(k) != 0)
				{
					//cout << "TU: " << buff << " " << j << " " << k << endl;
					s.replace(found, 10, to_string(jumpLocations -> at(k)) + "\n");
					//cout << nestedVector -> at(k) << " " << jumpLocations -> at(k) << endl;
					j++;
					break;
				}
			}
		}

		if(foundIterator != string::npos)
		{
			string number = s.substr(foundIterator + 13, 30);
			int i = stoi(number);
			//s.replace(found, 10, to_string(jumpLocations -> at(i)) + "\n");
			//cout << "J: " << j << endl;
			if(i == prevIt)
			{
				jIt--;
			}
			else
			{
				prevIt = i;
			}

			int val = nestedFor -> at(jIt);

			for(int k = jIt + 1; k < nestedFor -> size(); k++)
			{
				if(nestedFor -> at(k) == val && jumpForLocations -> at(k) == 0)
				{
					//cout << "break " << j << " " << k << endl;
					
					jIt++;
					k = jIt;
					continue;
					//break;
				}

				if(nestedFor -> at(k) < val)
				{					
					jIt++;
					k = jIt;
					val = nestedFor -> at(jIt);
					continue;
				}

				if(nestedFor -> at(k) == val && jumpForLocations -> at(k) != 0)
				{
					//cout << "TU: " << buff << " " << j << " " << k << endl;
					s.replace(foundIterator, 30, to_string(jumpForLocations -> at(k)) + "\n");
					//cout << nestedVector -> at(k) << " " << jumpLocations -> at(k) << endl;
					jIt++;
					break;
				}
			}
		}

		if(foundJumpIterator != string::npos)
		{
			string number = s.substr(foundJumpIterator + 13, 30);
			int i = stoi(number);
cout << "I: " << i << endl;
			if(i == prevJump)
			{
				jJump--;
			}
			else
			{
				prevJump = i;
			}

			int val = nestedForBack -> at(jJump);

			/*for(int k = jJump; k < nestedForBack -> size(); k++)
			{
cout << "val: " << val << " " << " k: " << k << " " << nestedForBack -> at(k) << " " << jumpForBackLocations -> at(k) << endl;

				if(nestedForBack -> at(k) == val && jumpForBackLocations -> at(k) == 0)
				{
					cout << "break " << jJump << " " << k << endl;
					
					k = jJump;
					jJump++;
					//k = jJump;
					continue;
					//break;
				}

				if(nestedForBack -> at(k) < val)
				{					
					jJump++;
					k = jJump;
					val = nestedForBack -> at(jJump);
					continue;
				}

				if(nestedForBack -> at(k) == val && jumpForBackLocations -> at(k) != 0)
				{
					cout << "TU: " << buff << " " << j << " " << k << endl;
					s.replace(foundJumpIterator, 30, to_string(jumpForBackLocations -> at(k)) + "\n");
					//cout << nestedVector -> at(k) << " " << jumpLocations -> at(k) << endl;
					jJump++;
					break;
				}
			}*/

			for(int k = jJump + 1; k < nestedForBack -> size(); k++)
			{
				if(val < nestedForBack -> at(k) && jumpForBackLocations -> at(k) == 0)
				{
					jJump++;
				}
				else if(val == nestedForBack -> at(k) && jumpForBackLocations -> at(k) != 0) //change
				{
					s.replace(foundJumpIterator, 30, to_string(jumpForBackLocations -> at(jumpForBackLocations -> size() - 1 - jJump)) + "\n");
					jJump++;
					break;
				}
				else if(val == nestedForBack -> at(k) && jumpForBackLocations -> at(k) == 0)
				{
					jJump++;
				}
				else
				{
					cout << jJump << " " << k << endl;
					s.replace(foundJumpIterator, 30, to_string(jumpForBackLocations -> at(k)) + "\n");
					jJump++;
					break;
				}
			}
		}

		if(foundWhile != string::npos)
		{
			string number = s.substr(foundWhile + 10, 30);
			int i = stoi(number);

			if(i == prevWhile)
			{
				jWhile--;
			}
			else
			{
				prevWhile = i;
			}

			int val = nestedWhileBack -> at(jWhile);

			for(int k = jWhile + 1; k < nestedWhileBack -> size(); k++)
			{
				if(val < nestedWhileBack -> at(k) && jumpWhileBackLocations -> at(k) == 0)
				{
					jWhile++;
				}
				else if(val == nestedWhileBack -> at(k) && jumpWhileBackLocations -> at(k) != 0)
				{
					s.replace(foundWhile, 30, to_string(jumpWhileBackLocations -> at(jumpWhileBackLocations -> size() - 1 - jWhile)) + "\n");
					jWhile++;
					break;
				}
				else if(val == nestedWhileBack -> at(k) && jumpWhileBackLocations -> at(k) == 0)
				{
					jWhile++;
				}
				else
				{
					cout << jWhile << " " << k << endl;
					s.replace(foundWhile, 30, to_string(jumpWhileBackLocations -> at(k)) + "\n");
					jWhile++;
					break;
				}
			}
		}

		char cString[s.size() + 1];
		strcpy(cString, s.c_str());
		fputs(cString, outFile);

		if(s == "HALT\n")
			break;
	}
}

void Compiler::setWhileFlag()
{
	inWhileFlag = true;
}

void Compiler::whileJump()
{
	if(inWhileFlag == true)
	{
		jumpWhileBackLocations -> push_back(generatedLines);
		nestedWhileBack -> push_back(ifNested);
		inWhileFlag = false;
	}
}

void Compiler::endWhile()
{
	string expr = "JUMP #whileFlag" + to_string(whileFlagNumber) + "\n";
	whileFlagNumber++;
	writeToFile(expr);

	inWhileFlag = false;
	nestedVector -> push_back(ifNested);
	ifNested--;
	jumpLocations -> push_back(generatedLines);

	jumpWhileBackLocations -> push_back(0);
	nestedWhileBack -> push_back(ifNested);
}

void Compiler::addJump()
{
	string expr = "JUMP #flag" + to_string(flagNumber) + "\n";
	flagNumber++;
	writeToFile(expr);
}

void Compiler::printNest()
{
	for(int i = 0; i < nestedVector -> size(); i++)
	{
		cout << i << " Nested: " << nestedVector -> at(i) << " location: " << jumpLocations -> at(i) << endl;
	}
	for(int i = 0; i < nestedFor -> size(); i++)
	{
		cout << i << " nest: " << nestedFor -> at(i) << " locations: " << jumpForLocations -> at(i) << endl;
	}

	for(int i = 0; i < nestedForBack -> size(); i++)
	{
		cout << i << " nest back: " << nestedForBack -> at(i) << " locations: " << jumpForBackLocations -> at(i) << endl;
	}

	for(int i = 0; i < nestedWhileBack -> size(); i++)
	{
		cout << i << " nest while back: " << nestedWhileBack -> at(i) << " locations: " << jumpWhileBackLocations -> at(i) << endl;
	}
}

void Compiler::endIf()
{
	//cout << "Nest: " << ifNested << endl;
	nestedVector -> push_back(ifNested);
	ifNested--;
	//cout << generatedLines << " " << flagNumber << endl;
	jumpLocations -> push_back(generatedLines);
}

void Compiler::endElse()
{
	//cout << "Nest: " << ifNested << endl;
	//ifNested--;
	nestedVector -> push_back(ifNested);
	//cout << generatedLines << " " << flagNumber << endl;
	jumpLocations -> push_back(generatedLines);
}

void Compiler::condEq()
{
	if(identifier == "")
	{
		string expr = "SUB 1\n";
		expr += "JNEG #flag" + to_string(flagNumber) + "\n";
		expr += "JPOS #flag" + to_string(flagNumber) + "\n";
		writeToFile(expr);
		clear();
		flagNumber++;
		jumpLocations -> push_back(0);
		ifNested++;
		nestedVector -> push_back(ifNested);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				string expr = "SUB 1\n";
				expr += "JNEG #flag" + to_string(flagNumber) + "\n";
				expr += "JPOS #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
			else
			{
				string expr = "SUB 1\n";
				expr += "JNEG #flag" + to_string(flagNumber) + "\n";
				expr += "JPOS #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			string expr = "SUB 1\n";
			expr += "JNEG #flag" + to_string(flagNumber) + "\n";
			expr += "JPOS #flag" + to_string(flagNumber) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}

	clear();
	flagNumber++;
	jumpLocations -> push_back(0);
	ifNested++;
	nestedVector -> push_back(ifNested);
}

void Compiler::condNeq()
{
	if(identifier == "")
	{
		string expr = "SUB 1\n";
		expr += "JZERO #flag" + to_string(flagNumber) + "\n";
		writeToFile(expr);
		clear();
		flagNumber++;
		jumpLocations -> push_back(0);
		ifNested++;
		nestedVector -> push_back(ifNested);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				string expr = "SUB 1\n";
				expr += "JZERO #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
			else
			{
				string expr = "SUB 1\n";
				expr += "JZERO #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			string expr = "SUB 1\n";
			expr += "JZERO #flag" + to_string(flagNumber) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}

	clear();
	flagNumber++;
	jumpLocations -> push_back(0);
	ifNested++;
	nestedVector -> push_back(ifNested);
}

void Compiler::condLe()
{
	if(identifier == "")
	{
		string expr = "SUB 1\n";
		expr += "JNEG #flag" + to_string(flagNumber) + "\n";
		expr += "JZERO #flag" + to_string(flagNumber) + "\n";
		writeToFile(expr);
		clear();
		flagNumber++;
		jumpLocations -> push_back(0);
		ifNested++;
		nestedVector -> push_back(ifNested);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				string expr = "SUB 1\n";
				expr += "JNEG #flag" + to_string(flagNumber) + "\n";
				expr += "JZERO #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
			else
			{
				string expr = "SUB 1\n";
				expr += "JNEG #flag" + to_string(flagNumber) + "\n";
				expr += "JZERO #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			string expr = "SUB 1\n";
			expr += "JNEG #flag" + to_string(flagNumber) + "\n";
			expr += "JZERO #flag" + to_string(flagNumber) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}

	clear();
	flagNumber++;
	jumpLocations -> push_back(0);
	ifNested++;
	nestedVector -> push_back(ifNested);
}

void Compiler::condGe()
{
	if(identifier == "")
	{
		string expr = "SUB 1 #geee\n";
		expr += "JPOS #flag" + to_string(flagNumber) + "\n";
		expr += "JZERO #flag" + to_string(flagNumber) + "\n";
		writeToFile(expr);
		clear();
		flagNumber++;
		jumpLocations -> push_back(0);
		ifNested++;
		nestedVector -> push_back(ifNested);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				string expr = "SUB 1\n";
				expr += "JPOS #flag" + to_string(flagNumber) + "\n";
				expr += "JZERO #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
			else
			{
				string expr = "SUB 1\n";
				expr += "JPOS #flag" + to_string(flagNumber) + "\n";
				expr += "JZERO #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			string expr = "SUB 1\n";
			expr += "JPOS #flag" + to_string(flagNumber) + "\n";
			expr += "JZERO #flag" + to_string(flagNumber) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}

	clear();
	flagNumber++;
	jumpLocations -> push_back(0);
	ifNested++;
	nestedVector -> push_back(ifNested);
}

void Compiler::condLeq()
{
	if(identifier == "")
	{
		string expr = "SUB 1\n";
		expr += "JNEG #flag" + to_string(flagNumber) + "\n";
		writeToFile(expr);
		clear();
		flagNumber++;
		jumpLocations -> push_back(0);
		ifNested++;
		nestedVector -> push_back(ifNested);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				string expr = "SUB 1\n";
				expr += "JNEG #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
			else
			{
				string expr = "SUB 1\n";
				expr += "JNEG #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			string expr = "SUB 1\n";
			expr += "JNEG #flag" + to_string(flagNumber) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}

	clear();
	flagNumber++;
	jumpLocations -> push_back(0);
	ifNested++;
	nestedVector -> push_back(ifNested);
}

void Compiler::condGeq()
{
	if(identifier == "")
	{
		string expr = "SUB 1\n";
		expr += "JPOS #flag" + to_string(flagNumber) + "\n";
		writeToFile(expr);
		clear();
		flagNumber++;
		jumpLocations -> push_back(0);
		ifNested++;
		nestedVector -> push_back(ifNested);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				string expr = "SUB 1\n";
				expr += "JPOS #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
			else
			{
				string expr = "SUB 1\n";
				expr += "JNEG #flag" + to_string(flagNumber) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			string expr = "SUB 1\n";
			expr += "JPOS #flag" + to_string(flagNumber) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}

	clear();
	flagNumber++;
	jumpLocations -> push_back(0);
	ifNested++;
	nestedVector -> push_back(ifNested);
}

void Compiler::add()
{
	if(identifier == "")
	{
		string expr = "ADD 1\n";
		writeToFile(expr);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				string expr = "ADD 1\n";
				writeToFile(expr);
			}
			else
			{
				string expr = "ADD 1\n";
				writeToFile(expr);
			}
		}
		else
		{
			//long long memoryLocation = symbolsTable -> getVarMemoryLocation(identifier);

			string expr = "ADD 1\n"; /* + to_string(memoryLocation) + "\n";*/
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

//same as add function, but "ADD 1" changed to "SUB 1" and parameters need to change places,
//because left side is stored in p1 and right one in p0
void Compiler::sub() 
{
	long long freeCell = symbolsTable -> getFreeMemoryCell();

	if(identifier == "")
	{
		string expr = "STORE " + to_string(freeCell) + "\n";
		expr +=  "LOAD 1\nSUB " + to_string(freeCell) + "\n";
		writeToFile(expr);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				string expr = "STORE " + to_string(freeCell) + "\n";
				expr +=  "LOAD 1\nSUB " + to_string(freeCell) + "\n";
				writeToFile(expr);
			}
			else
			{
				string expr = "STORE " + to_string(freeCell) + "\n";
				expr +=  "LOAD 1\nSUB " + to_string(freeCell) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			string expr = "STORE " + to_string(freeCell) + "\n";
				expr +=  "LOAD 1\nSUB " + to_string(freeCell) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

void Compiler::mul()
{
	long long freeCell = symbolsTable -> getFreeMemoryCell();

	if(identifier == "")
	{
		int num = generatedLines;
		string expr = "SUB 1\n";
		expr += "JNEG " + to_string(num + 10) + "\n";
		expr += "ADD 1\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD 1\n";
		expr += "STORE " + to_string(freeCell) + "\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "STORE 1\n";
		expr += "LOAD " + to_string(freeCell) + "\n";
		expr += "JUMP " + to_string(num + 11) + "\n";
		expr += "ADD 1\n";
		writeToFile(expr);

		int num2 = generatedLines;
		expr = "STORE " + to_string(freeCell) + "\n";
		expr += "SUB 0\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD " + to_string(freeCell) + "\n";
		expr += "JNEG " + to_string(num2 + 15) + "\n";
		writeToFile(expr);

		int line = generatedLines;
		expr = "JZERO " + to_string(generatedLines + 8) + "\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "ADD 1\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD " + to_string(freeCell) + "\n";
		expr += "DEC\n";
		expr += "STORE " + to_string(freeCell) + "\n";
		expr += "JUMP " + to_string(generatedLines) + "\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "JUMP " + to_string(generatedLines + 8 + 11) + "\n";
		writeToFile(expr);

		int l = generatedLines;
		expr = "JZERO " + to_string(l + 8) + "\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "SUB 1\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD " + to_string(freeCell) + "\n";
		expr += "INC\n";
		expr += "STORE " + to_string(freeCell) + "\n";
		expr += "JUMP " + to_string(l) + "\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		writeToFile(expr);

		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				int num = generatedLines;
				string expr = "SUB 1\n";
				expr += "JNEG " + to_string(num + 10) + "\n";
				expr += "ADD 1\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "STORE " + to_string(freeCell) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JUMP " + to_string(num + 11) + "\n";
				expr += "ADD 1\n";
				writeToFile(expr);

				int num2 = generatedLines;

				expr = "STORE " + to_string(freeCell) + "\n";
				expr += "SUB 0\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JNEG " + to_string(num2 + 15) + "\n";
				writeToFile(expr);

				int line = generatedLines;
				expr = "JZERO " + to_string(generatedLines + 8) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "ADD 1\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell) + "\n";
				expr += "JUMP " + to_string(generatedLines) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(generatedLines + 8 + 11) + "\n";
				writeToFile(expr);

				int l = generatedLines;
				expr = "JZERO " + to_string(l + 8) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "SUB 1\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell) + "\n";
				expr += "JUMP " + to_string(l) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				writeToFile(expr);
			}
			else
			{
				int num = generatedLines;
				string expr = "SUB 1\n";
				expr += "JNEG " + to_string(num + 10) + "\n";
				expr += "ADD 1\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "STORE " + to_string(freeCell) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JUMP " + to_string(num + 11) + "\n";
				expr += "ADD 1\n";
				writeToFile(expr);

				int num2 = generatedLines;

				expr = "STORE " + to_string(freeCell) + "\n";
				expr += "SUB 0\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JNEG " + to_string(num2 + 15) + "\n";
				writeToFile(expr);

				int line = generatedLines;
				expr = "JZERO " + to_string(generatedLines + 8) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "ADD 1\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell) + "\n";
				expr += "JUMP " + to_string(generatedLines) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(generatedLines + 8 + 11) + "\n";
				writeToFile(expr);

				int l = generatedLines;
				expr = "JZERO " + to_string(l + 8) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "SUB 1\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell) + "\n";
				expr += "JUMP " + to_string(l) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			int num = generatedLines;
			string expr = "SUB 1\n";
			expr += "JNEG " + to_string(num + 10) + "\n";
			expr += "ADD 1\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD 1\n";
			expr += "STORE " + to_string(freeCell) + "\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "STORE 1\n";
			expr += "LOAD " + to_string(freeCell) + "\n";
			expr += "JUMP " + to_string(num + 11) + "\n";
			expr += "ADD 1\n";
			writeToFile(expr);

			int num2 = generatedLines;

			expr = "STORE " + to_string(freeCell) + "\n";
			expr += "SUB 0\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD " + to_string(freeCell) + "\n";
			expr += "JNEG " + to_string(num2 + 15) + "\n";
			writeToFile(expr);

			int line = generatedLines;
			expr = "JZERO " + to_string(generatedLines + 8) + "\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "ADD 1\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD " + to_string(freeCell) + "\n";
			expr += "DEC\n";
			expr += "STORE " + to_string(freeCell) + "\n";
			expr += "JUMP " + to_string(generatedLines) + "\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "JUMP " + to_string(generatedLines + 8 + 11) + "\n";
			writeToFile(expr);

			int l = generatedLines;
			expr = "JZERO " + to_string(l + 8) + "\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "SUB 1\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD " + to_string(freeCell) + "\n";
			expr += "INC\n";
			expr += "STORE " + to_string(freeCell) + "\n";
			expr += "JUMP " + to_string(l) + "\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

void Compiler::div()
{
	long long freeCell = symbolsTable -> getFreeMemoryCell();

	if(identifier == "")
	{
		string expr2 = "STORE " + to_string(freeCell) + " #div---------\n";
		expr2 += "SUB 0\n";
		expr2 += "INC\n";
		expr2 += "INC\n";
		expr2 += "SUB " + to_string(freeCell) + "\n";
		expr2 += "JNEG " + to_string(generatedLines + 14) + "\n";
		expr2 += "JPOS " + to_string(generatedLines + 14) + "\n";
		expr2 += "SUB 0\n";
		expr2 += "DEC\n";
		expr2 += "STORE " + to_string(freeCell + 2) + "\n";
		expr2 += "LOAD 1\n";
		expr2 += "SHIFT " + to_string(freeCell + 2) + "\n";
		expr2 += "STORE 1\n";
		//expr2 += "LOAD " + to_string(freeCell) + "\n";

		writeToFile(expr2);
		expr2 = "JUMP " + to_string(generatedLines + 46 + 23) + "\n";
		expr2 += "LOAD " + to_string(freeCell) + "\n";
		writeToFile(expr2);

		//decision tree
		string expr = "STORE " + to_string(freeCell) + "\n";
		expr += "SUB 0\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD " + to_string(freeCell) + "\n";
		expr += "JZERO " + to_string(generatedLines + 67) + "\n"; //div by 0

		expr += "JNEG " + to_string(generatedLines + 7) + "\n"; //right side is negative
		expr += "JUMP " + to_string(generatedLines + 10) + "\n"; //right side is positive

		expr += "LOAD 1\n"; //load left side
		expr += "JNEG " + to_string(generatedLines + 52) + "\n"; //left is neg, right is neg
		expr += "JUMP " + to_string(generatedLines + 28) + "\n"; //left is pos, right is neg

		expr += "LOAD 1\n";
		expr += "JNEG " + to_string(generatedLines + 40) + "\n"; //left is neg, right is pos
		//left is pos, right is pos	

		expr += "LOAD " + to_string(freeCell) + "\n";

		writeToFile(expr);

		//left side is positive, right side is positive
		//18 / 5
		int l1 = generatedLines;
		expr = "LOAD 1\n";
		expr += "JZERO " + to_string(l1 + 13) + "\n";
		expr += "JNEG " + to_string(l1 + 10) + "\n";
		expr += "SUB " + to_string(freeCell) + "\n";
		expr += "STORE 1\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "INC\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD 1\n";
		expr += "JUMP " + to_string(l1 + 1) + "\n";

		expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
		expr += "DEC\n";
		//jneg additional jump to omit load beneath
		expr += "JUMP " + to_string(l1 + 14) + "\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "JUMP " + to_string(l1 + 54) + "\n"; //skip other variants
		writeToFile(expr);

		//left side is positive, right side is negative
		//18 / -5
		int l2 = generatedLines;
		expr = "LOAD 1\n";
		expr += "JZERO " + to_string(l2 + 10) + "\n";
		expr += "JNEG " + to_string(l2 + 10) + "\n";
		expr += "ADD " + to_string(freeCell) + "\n";
		expr += "STORE 1\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "DEC\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD 1\n";
		expr += "JUMP " + to_string(l2 + 1) + "\n";

		//expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
		//expr += "INC\n";
		//expr += "JUMP " + to_string(l2 + 14) + "\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "JUMP " + to_string(l2 + 39) + "\n"; //skip other variants
		writeToFile(expr);

		//left side is negative, right side is positive
		//-18 / 5
		int l3 = generatedLines;
		expr = "LOAD 1\n";
		expr += "JZERO " + to_string(l3 + 10) + "\n";
		expr += "JPOS " + to_string(l3 + 10) + "\n";
		expr += "ADD " + to_string(freeCell) + "\n";
		expr += "STORE 1\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "DEC\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD 1\n";
		expr += "JUMP " + to_string(l3 + 1) + "\n";

		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "JUMP " + to_string(l3 + 27) + "\n"; //skip other variants
		writeToFile(expr);

		//left side is negative, right side is negative
		//-18 / -5
		int l4 = generatedLines;
		expr = "LOAD 1\n";
		expr += "JZERO " + to_string(l4 + 13) + "\n";
		expr += "JPOS " + to_string(l4 + 10) + "\n";
		expr += "SUB " + to_string(freeCell) + "\n";
		expr += "STORE 1\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "INC\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD 1\n";
		expr += "JUMP " + to_string(l4 + 1) + "\n";

		expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
		expr += "DEC\n";
		expr += "JUMP " + to_string(l4 + 14) + "\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "JUMP " + to_string(l4 + 15) + "\n";
		writeToFile(expr);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				symbolsTable -> checkIfDeclared(indexIdentifier);
				symbolsTable -> checkIfInitialized(indexIdentifier);

				string expr2 = "STORE " + to_string(freeCell) + " #div---------\n";
				expr2 += "SUB 0\n";
				expr2 += "INC\n";
				expr2 += "INC\n";
				expr2 += "SUB " + to_string(freeCell) + "\n";
				expr2 += "JNEG " + to_string(generatedLines + 14) + "\n";
				expr2 += "JPOS " + to_string(generatedLines + 14) + "\n";
				expr2 += "SUB 0\n";
				expr2 += "DEC\n";
				expr2 += "STORE " + to_string(freeCell + 2) + "\n";
				expr2 += "LOAD 1\n";
				expr2 += "SHIFT " + to_string(freeCell + 2) + "\n";
				expr2 += "STORE 1\n";
				//expr2 += "LOAD " + to_string(freeCell) + "\n";

				writeToFile(expr2);
				expr2 = "JUMP " + to_string(generatedLines + 46 + 23) + "\n";
				expr2 += "LOAD " + to_string(freeCell) + "\n";
				writeToFile(expr2);

				//decision tree
				string expr = "STORE " + to_string(freeCell) + "\n";
				expr += "SUB 0\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JZERO " + to_string(generatedLines + 67) + "\n"; //div by 0

				expr += "JNEG " + to_string(generatedLines + 7) + "\n"; //right side is negative
				expr += "JUMP " + to_string(generatedLines + 10) + "\n"; //right side is positive

				expr += "LOAD 1\n"; //load left side
				expr += "JNEG " + to_string(generatedLines + 52) + "\n"; //left is neg, right is neg
				expr += "JUMP " + to_string(generatedLines + 28) + "\n"; //left is pos, right is neg

				expr += "LOAD 1\n";
				expr += "JNEG " + to_string(generatedLines + 40) + "\n"; //left is neg, right is pos
				//left is pos, right is pos	

				expr += "LOAD " + to_string(freeCell) + "\n";

				writeToFile(expr);

				//left side is positive, right side is positive
				//18 / 5
				int l1 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l1 + 13) + "\n";
				expr += "JNEG " + to_string(l1 + 10) + "\n";
				expr += "SUB " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l1 + 1) + "\n";

				expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
				expr += "DEC\n";
				//jneg additional jump to omit load beneath
				expr += "JUMP " + to_string(l1 + 14) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l1 + 54) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is positive, right side is negative
				//18 / -5
				int l2 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l2 + 10) + "\n";
				expr += "JNEG " + to_string(l2 + 10) + "\n";
				expr += "ADD " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l2 + 1) + "\n";

				//expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
				//expr += "INC\n";
				//expr += "JUMP " + to_string(l2 + 14) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l2 + 39) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is negative, right side is positive
				//-18 / 5
				int l3 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l3 + 10) + "\n";
				expr += "JPOS " + to_string(l3 + 10) + "\n";
				expr += "ADD " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l3 + 1) + "\n";

				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l3 + 27) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is negative, right side is negative
				//-18 / -5
				int l4 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l4 + 13) + "\n";
				expr += "JPOS " + to_string(l4 + 10) + "\n";
				expr += "SUB " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l4 + 1) + "\n";

				expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
				expr += "DEC\n";
				expr += "JUMP " + to_string(l4 + 14) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l4 + 15) + "\n";
				writeToFile(expr);
			}
			else
			{
				string expr2 = "STORE " + to_string(freeCell) + " #div---------\n";
				expr2 += "SUB 0\n";
				expr2 += "INC\n";
				expr2 += "INC\n";
				expr2 += "SUB " + to_string(freeCell) + "\n";
				expr2 += "JNEG " + to_string(generatedLines + 14) + "\n";
				expr2 += "JPOS " + to_string(generatedLines + 14) + "\n";
				expr2 += "SUB 0\n";
				expr2 += "DEC\n";
				expr2 += "STORE " + to_string(freeCell + 2) + "\n";
				expr2 += "LOAD 1\n";
				expr2 += "SHIFT " + to_string(freeCell + 2) + "\n";
				expr2 += "STORE 1\n";
				//expr2 += "LOAD " + to_string(freeCell) + "\n";

				writeToFile(expr2);
				expr2 = "JUMP " + to_string(generatedLines + 46 + 23) + "\n";
				expr2 += "LOAD " + to_string(freeCell) + "\n";
				writeToFile(expr2);

				//decision tree
				string expr = "STORE " + to_string(freeCell) + "\n";
				expr += "SUB 0\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JZERO " + to_string(generatedLines + 67) + "\n"; //div by 0

				expr += "JNEG " + to_string(generatedLines + 7) + "\n"; //right side is negative
				expr += "JUMP " + to_string(generatedLines + 10) + "\n"; //right side is positive

				expr += "LOAD 1\n"; //load left side
				expr += "JNEG " + to_string(generatedLines + 52) + "\n"; //left is neg, right is neg
				expr += "JUMP " + to_string(generatedLines + 28) + "\n"; //left is pos, right is neg

				expr += "LOAD 1\n";
				expr += "JNEG " + to_string(generatedLines + 40) + "\n"; //left is neg, right is pos
				//left is pos, right is pos	

				expr += "LOAD " + to_string(freeCell) + "\n";

				writeToFile(expr);

				//left side is positive, right side is positive
				//18 / 5
				int l1 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l1 + 13) + "\n";
				expr += "JNEG " + to_string(l1 + 10) + "\n";
				expr += "SUB " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l1 + 1) + "\n";

				expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
				expr += "DEC\n";
				//jneg additional jump to omit load beneath
				expr += "JUMP " + to_string(l1 + 14) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l1 + 54) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is positive, right side is negative
				//18 / -5
				int l2 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l2 + 10) + "\n";
				expr += "JNEG " + to_string(l2 + 10) + "\n";
				expr += "ADD " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l2 + 1) + "\n";

				//expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
				//expr += "INC\n";
				//expr += "JUMP " + to_string(l2 + 14) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l2 + 39) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is negative, right side is positive
				//-18 / 5
				int l3 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l3 + 10) + "\n";
				expr += "JPOS " + to_string(l3 + 10) + "\n";
				expr += "ADD " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l3 + 1) + "\n";

				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l3 + 27) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is negative, right side is negative
				//-18 / -5
				int l4 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l4 + 13) + "\n";
				expr += "JPOS " + to_string(l4 + 10) + "\n";
				expr += "SUB " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l4 + 1) + "\n";

				expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
				expr += "DEC\n";
				expr += "JUMP " + to_string(l4 + 14) + "\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l4 + 15) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			string expr2 = "STORE " + to_string(freeCell) + " #div---------\n";
			expr2 += "SUB 0\n";
			expr2 += "INC\n";
			expr2 += "INC\n";
			expr2 += "SUB " + to_string(freeCell) + "\n";
			expr2 += "JNEG " + to_string(generatedLines + 14) + "\n";
			expr2 += "JPOS " + to_string(generatedLines + 14) + "\n";
			expr2 += "SUB 0\n";
			expr2 += "DEC\n";
			expr2 += "STORE " + to_string(freeCell + 2) + "\n";
			expr2 += "LOAD 1\n";
			expr2 += "SHIFT " + to_string(freeCell + 2) + "\n";
			expr2 += "STORE 1\n";
			//expr2 += "LOAD " + to_string(freeCell) + "\n";

			writeToFile(expr2);
			expr2 = "JUMP " + to_string(generatedLines + 46 + 23) + "\n";
			expr2 += "LOAD " + to_string(freeCell) + "\n";
			writeToFile(expr2);

			//decision tree
			string expr = "STORE " + to_string(freeCell) + "\n";
			expr += "SUB 0\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD " + to_string(freeCell) + "\n";
			expr += "JZERO " + to_string(generatedLines + 67) + "\n"; //div by 0

			expr += "JNEG " + to_string(generatedLines + 7) + "\n"; //right side is negative
			expr += "JUMP " + to_string(generatedLines + 10) + "\n"; //right side is positive

			expr += "LOAD 1\n"; //load left side
			expr += "JNEG " + to_string(generatedLines + 52) + "\n"; //left is neg, right is neg
			expr += "JUMP " + to_string(generatedLines + 28) + "\n"; //left is pos, right is neg

			expr += "LOAD 1\n";
			expr += "JNEG " + to_string(generatedLines + 40) + "\n"; //left is neg, right is pos
			//left is pos, right is pos	

			expr += "LOAD " + to_string(freeCell) + "\n";

			writeToFile(expr);

			//left side is positive, right side is positive
			//18 / 5
			int l1 = generatedLines;
			expr = "LOAD 1\n";
			expr += "JZERO " + to_string(l1 + 13) + "\n";
			expr += "JNEG " + to_string(l1 + 10) + "\n";
			expr += "SUB " + to_string(freeCell) + "\n";
			expr += "STORE 1\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "INC\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD 1\n";
			expr += "JUMP " + to_string(l1 + 1) + "\n";

			expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
			expr += "DEC\n";
			//jneg additional jump to omit load beneath
			expr += "JUMP " + to_string(l1 + 14) + "\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "JUMP " + to_string(l1 + 54) + "\n"; //skip other variants
			writeToFile(expr);

			//left side is positive, right side is negative
			//18 / -5
			int l2 = generatedLines;
			expr = "LOAD 1\n";
			expr += "JZERO " + to_string(l2 + 10) + "\n";
			expr += "JNEG " + to_string(l2 + 10) + "\n";
			expr += "ADD " + to_string(freeCell) + "\n";
			expr += "STORE 1\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "DEC\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD 1\n";
			expr += "JUMP " + to_string(l2 + 1) + "\n";

			//expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
			//expr += "INC\n";
			//expr += "JUMP " + to_string(l2 + 14) + "\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "JUMP " + to_string(l2 + 39) + "\n"; //skip other variants
			writeToFile(expr);

			//left side is negative, right side is positive
			//-18 / 5
			int l3 = generatedLines;
			expr = "LOAD 1\n";
			expr += "JZERO " + to_string(l3 + 10) + "\n";
			expr += "JPOS " + to_string(l3 + 10) + "\n";
			expr += "ADD " + to_string(freeCell) + "\n";
			expr += "STORE 1\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "DEC\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD 1\n";
			expr += "JUMP " + to_string(l3 + 1) + "\n";

			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "JUMP " + to_string(l3 + 27) + "\n"; //skip other variants
			writeToFile(expr);

			//left side is negative, right side is negative
			//-18 / -5
			int l4 = generatedLines;
			expr = "LOAD 1\n";
			expr += "JZERO " + to_string(l4 + 13) + "\n";
			expr += "JPOS " + to_string(l4 + 10) + "\n";
			expr += "SUB " + to_string(freeCell) + "\n";
			expr += "STORE 1\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "INC\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD 1\n";
			expr += "JUMP " + to_string(l4 + 1) + "\n";

			expr += "LOAD " + to_string(freeCell + 1) + "\n"; //without this - mod
			expr += "DEC\n";
			expr += "JUMP " + to_string(l4 + 14) + "\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "JUMP " + to_string(l4 + 15) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

void Compiler::mod()
{
	long long freeCell = symbolsTable -> getFreeMemoryCell();

	if(identifier == "")
	{
		string expr2 = "STORE " + to_string(freeCell) + " #mod---------\n";
		expr2 += "SUB 0\n";
		expr2 += "INC\n";
		expr2 += "INC\n";
		expr2 += "SUB " + to_string(freeCell) + "\n";
		expr2 += "JNEG " + to_string(generatedLines + 23) + "\n";
		expr2 += "JPOS " + to_string(generatedLines + 23) + "\n";
		expr2 += "SUB 0\n";
		expr2 += "DEC\n";
		expr2 += "STORE " + to_string(freeCell + 1) + "\n";
		expr2 += "SUB 0\n";
		expr2 += "INC\n";
		expr2 += "STORE " + to_string(freeCell + 2) + "\n";
		expr2 += "LOAD 1\n";
		expr2 += "SHIFT " + to_string(freeCell + 1) + "\n";
		expr2 += "SHIFT " + to_string(freeCell + 2) + "\n";
		expr2 += "SUB 1\n";
		expr2 += "JZERO " + to_string(generatedLines + 21) + "\n";
		expr2 += "STORE 1\n";

		expr2 += "LOAD " + to_string(freeCell + 2) + "\n";
		expr2 += "JUMP " + to_string(generatedLines + 22) + "\n";
		expr2 += "STORE 1\n";
		writeToFile(expr2);
		//expr2 = "LOAD " + to_string(freeCell) + "\n";
		expr2 = "JUMP " + to_string(generatedLines + 46 + 15) + "\n";
		expr2 += "LOAD " + to_string(freeCell) + "\n";
		writeToFile(expr2);

		//one-time optimization TODO
		/*string expr2 = "STORE " + to_string(freeCell) + " #modddddd----\n";
		expr2 += "STORE " + to_string(freeCell + 2) + "\n";
		expr2 += "SUB 0\n";
		expr2 += "INC\n";
		expr2 += "STORE " + to_string(freeCell + 1) + "\n";
		expr2 += "LOAD " + to_string(freeCell) + "\n";
		expr2 += "SUB 1\n";
		expr2 += "JPOS " + to_string(generatedLines + 16) + "\n";
		expr2 += "ADD 1\n";
		expr2 += "ADD 0\n";
		expr2 += "STORE " + to_string(freeCell) + "\n";
		expr2 += "LOAD " + to_string(freeCell + 1) + "\n";
		expr2 += "INC\n";
		expr2 += "STORE " + to_string(freeCell + 1) + "\n";
		expr2 += "LOAD " + to_string(freeCell) + "\n";
		expr2 += "JUMP " + to_string(generatedLines + 6) + "\n";

		//expr2 += "LOAD " + to_string(freeCell + 1) + "\n";
		//expr2 += "DEC\n";
		//expr2 += "STORE " + to_string(freeCell + 1) + "\n";
		//expr2 += "SUB 0\n";
		//expr2 += "SUB " + to_string(freeCell + 1) + "\n";
		//expr2 += "STORE " + to_string(freeCell + 1) + "\n";
		expr2 += "SUB 0\n";
		expr2 += "DEC\n";
		expr2 += "STORE " + to_string(freeCell + 1) + "\n";
		expr2 += "LOAD " + to_string(freeCell) + "\n";
		expr2 += "SHIFT " + to_string(freeCell + 1) + "\n";
		expr2 += "STORE " + to_string(freeCell) + "\n";
		
		expr2 += "LOAD 1\n";
		//expr2 += "SHIFT " + to_string(freeCell + 1) + "\n";
		expr2 += "SUB " + to_string(freeCell) + "\n";
		expr2 += "STORE 1\n";
		expr2 += "LOAD " + to_string(freeCell + 2) + "\n";*/
		
		//writeToFile(expr2);

		//decision tree
		string expr = "STORE " + to_string(freeCell) + "\n";
		expr += "SUB 0\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD " + to_string(freeCell) + "\n";
		expr += "JZERO " + to_string(generatedLines + 59) + "\n"; //div by 0

		expr += "JNEG " + to_string(generatedLines + 7) + "\n"; //right side is negative
		expr += "JUMP " + to_string(generatedLines + 10) + "\n"; //right side is positive

		expr += "LOAD 1\n"; //load left side
		expr += "JNEG " + to_string(generatedLines + 48) + "\n"; //left is neg, right is neg
		expr += "JUMP " + to_string(generatedLines + 26) + "\n"; //left is pos, right is neg

		expr += "LOAD 1\n";
		expr += "JNEG " + to_string(generatedLines + 37) + "\n"; //left is neg, right is pos
		//left is pos, right is pos	

		expr += "LOAD " + to_string(freeCell) + "\n";

		writeToFile(expr);

		//left side is positive, right side is positive
		//18 / 5
		int l1 = generatedLines;
		expr = "LOAD 1\n";
		expr += "JZERO " + to_string(l1 + 11) + "\n";
		expr += "JNEG " + to_string(l1 + 10) + "\n";
		expr += "SUB " + to_string(freeCell) + "\n";
		expr += "STORE 1\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "INC\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD 1\n";
		expr += "JUMP " + to_string(l1 + 1) + "\n";

		expr += "ADD " + to_string(freeCell) + "\n";
		//expr += "DEC\n";
		//jneg additional jump to omit load beneath
		//expr += "JUMP " + to_string(l1 + 14) + "\n";
		//expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "JUMP " + to_string(l1 + 46) + "\n"; //skip other variants
		writeToFile(expr);

		//left side is positive, right side is negative
		//18 / -5
		int l2 = generatedLines;
		expr = "LOAD 1\n";
		expr += "JZERO " + to_string(l2 + 10) + "\n";
		expr += "JNEG " + to_string(l2 + 10) + "\n";
		expr += "ADD " + to_string(freeCell) + "\n";
		expr += "STORE 1\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "DEC\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD 1\n";
		expr += "JUMP " + to_string(l2 + 1) + "\n";

		//expr += "SUB " + to_string(freeCell) + "\n";
		//expr += "INC\n";
		//expr += "JUMP " + to_string(l2 + 14) + "\n";
		//expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "JUMP " + to_string(l2 + 34) + "\n"; //skip other variants
		writeToFile(expr);

		//left side is negative, right side is positive
		//-18 / 5
		int l3 = generatedLines;
		expr = "LOAD 1\n";
		expr += "JZERO " + to_string(l3 + 10) + "\n";
		expr += "JPOS " + to_string(l3 + 10) + "\n";
		expr += "ADD " + to_string(freeCell) + "\n";
		expr += "STORE 1\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "DEC\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD 1\n";
		expr += "JUMP " + to_string(l3 + 1) + "\n";

		//expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "JUMP " + to_string(l3 + 23) + "\n"; //skip other variants
		writeToFile(expr);

		//left side is negative, right side is negative
		//-18 / -5
		int l4 = generatedLines;
		expr = "LOAD 1\n";
		expr += "JZERO " + to_string(l4 + 11) + "\n";
		expr += "JPOS " + to_string(l4 + 10) + "\n";
		expr += "SUB " + to_string(freeCell) + "\n";
		expr += "STORE 1\n";
		expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "INC\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD 1\n";
		expr += "JUMP " + to_string(l4 + 1) + "\n";

		expr += "ADD " + to_string(freeCell) + "\n";
		//expr += "DEC\n";
		//expr += "JUMP " + to_string(l4 + 14) + "\n";
		//expr += "LOAD " + to_string(freeCell + 1) + "\n";
		expr += "JUMP " + to_string(l4 + 12) + " #koniec\n";
		writeToFile(expr);
		return;
	}

	try
	{
		if(symbolsTable -> checkIfArray(identifier) == true)
		{
			if(indexIdentifier != "")
			{
				string expr2 = "STORE " + to_string(freeCell) + " #mod---------\n";
				expr2 += "SUB 0\n";
				expr2 += "INC\n";
				expr2 += "INC\n";
				expr2 += "SUB " + to_string(freeCell) + "\n";
				expr2 += "JNEG " + to_string(generatedLines + 23) + "\n";
				expr2 += "JPOS " + to_string(generatedLines + 23) + "\n";
				expr2 += "SUB 0\n";
				expr2 += "DEC\n";
				expr2 += "STORE " + to_string(freeCell + 1) + "\n";
				expr2 += "SUB 0\n";
				expr2 += "INC\n";
				expr2 += "STORE " + to_string(freeCell + 2) + "\n";
				expr2 += "LOAD 1\n";
				expr2 += "SHIFT " + to_string(freeCell + 1) + "\n";
				expr2 += "SHIFT " + to_string(freeCell + 2) + "\n";
				expr2 += "SUB 1\n";
				expr2 += "JZERO " + to_string(generatedLines + 21) + "\n";
				expr2 += "STORE 1\n";

				expr2 += "LOAD " + to_string(freeCell + 2) + "\n";
				expr2 += "JUMP " + to_string(generatedLines + 22) + "\n";
				expr2 += "STORE 1\n";
				writeToFile(expr2);
				//expr2 = "LOAD " + to_string(freeCell) + "\n";
				expr2 = "JUMP " + to_string(generatedLines + 46 + 15) + "\n";
				expr2 += "LOAD " + to_string(freeCell) + "\n";
				writeToFile(expr2);

				//decision tree
				string expr = "STORE " + to_string(freeCell) + "\n";
				expr += "SUB 0\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JZERO " + to_string(generatedLines + 59) + "\n"; //div by 0

				expr += "JNEG " + to_string(generatedLines + 7) + "\n"; //right side is negative
				expr += "JUMP " + to_string(generatedLines + 10) + "\n"; //right side is positive

				expr += "LOAD 1\n"; //load left side
				expr += "JNEG " + to_string(generatedLines + 48) + "\n"; //left is neg, right is neg
				expr += "JUMP " + to_string(generatedLines + 26) + "\n"; //left is pos, right is neg

				expr += "LOAD 1\n";
				expr += "JNEG " + to_string(generatedLines + 37) + "\n"; //left is neg, right is pos
				//left is pos, right is pos	

				expr += "LOAD " + to_string(freeCell) + "\n";

				writeToFile(expr);

				//left side is positive, right side is positive
				//18 / 5
				int l1 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l1 + 11) + "\n";
				expr += "JNEG " + to_string(l1 + 10) + "\n";
				expr += "SUB " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l1 + 1) + "\n";

				expr += "ADD " + to_string(freeCell) + "\n";
				//expr += "DEC\n";
				//jneg additional jump to omit load beneath
				//expr += "JUMP " + to_string(l1 + 14) + "\n";
				//expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l1 + 46) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is positive, right side is negative
				//18 / -5
				int l2 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l2 + 10) + "\n";
				expr += "JNEG " + to_string(l2 + 10) + "\n";
				expr += "ADD " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l2 + 1) + "\n";

				//expr += "SUB " + to_string(freeCell) + "\n";
				//expr += "INC\n";
				//expr += "JUMP " + to_string(l2 + 14) + "\n";
				//expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l2 + 34) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is negative, right side is positive
				//-18 / 5
				int l3 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l3 + 10) + "\n";
				expr += "JPOS " + to_string(l3 + 10) + "\n";
				expr += "ADD " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l3 + 1) + "\n";

				//expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l3 + 23) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is negative, right side is negative
				//-18 / -5
				int l4 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l4 + 11) + "\n";
				expr += "JPOS " + to_string(l4 + 10) + "\n";
				expr += "SUB " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l4 + 1) + "\n";

				expr += "ADD " + to_string(freeCell) + "\n";
				//expr += "DEC\n";
				//expr += "JUMP " + to_string(l4 + 14) + "\n";
				//expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l4 + 12) + "\n";
				writeToFile(expr);
			}
			else
			{
				string expr2 = "STORE " + to_string(freeCell) + " #mod---------\n";
				expr2 += "SUB 0\n";
				expr2 += "INC\n";
				expr2 += "INC\n";
				expr2 += "SUB " + to_string(freeCell) + "\n";
				expr2 += "JNEG " + to_string(generatedLines + 23) + "\n";
				expr2 += "JPOS " + to_string(generatedLines + 23) + "\n";
				expr2 += "SUB 0\n";
				expr2 += "DEC\n";
				expr2 += "STORE " + to_string(freeCell + 1) + "\n";
				expr2 += "SUB 0\n";
				expr2 += "INC\n";
				expr2 += "STORE " + to_string(freeCell + 2) + "\n";
				expr2 += "LOAD 1\n";
				expr2 += "SHIFT " + to_string(freeCell + 1) + "\n";
				expr2 += "SHIFT " + to_string(freeCell + 2) + "\n";
				expr2 += "SUB 1\n";
				expr2 += "JZERO " + to_string(generatedLines + 21) + "\n";
				expr2 += "STORE 1\n";

				expr2 += "LOAD " + to_string(freeCell + 2) + "\n";
				expr2 += "JUMP " + to_string(generatedLines + 22) + "\n";
				expr2 += "STORE 1\n";
				writeToFile(expr2);
				//expr2 = "LOAD " + to_string(freeCell) + "\n";
				expr2 = "JUMP " + to_string(generatedLines + 46 + 15) + "\n";
				expr2 += "LOAD " + to_string(freeCell) + "\n";
				writeToFile(expr2);

				//decision tree
				string expr = "STORE " + to_string(freeCell) + "\n";
				expr += "SUB 0\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JZERO " + to_string(generatedLines + 59) + "\n"; //div by 0

				expr += "JNEG " + to_string(generatedLines + 7) + "\n"; //right side is negative
				expr += "JUMP " + to_string(generatedLines + 10) + "\n"; //right side is positive

				expr += "LOAD 1\n"; //load left side
				expr += "JNEG " + to_string(generatedLines + 48) + "\n"; //left is neg, right is neg
				expr += "JUMP " + to_string(generatedLines + 26) + "\n"; //left is pos, right is neg

				expr += "LOAD 1\n";
				expr += "JNEG " + to_string(generatedLines + 37) + "\n"; //left is neg, right is pos
				//left is pos, right is pos	

				expr += "LOAD " + to_string(freeCell) + "\n";

				writeToFile(expr);

				//left side is positive, right side is positive
				//18 / 5
				int l1 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l1 + 11) + "\n";
				expr += "JNEG " + to_string(l1 + 10) + "\n";
				expr += "SUB " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l1 + 1) + "\n";

				expr += "ADD " + to_string(freeCell) + "\n";
				//expr += "DEC\n";
				//jneg additional jump to omit load beneath
				//expr += "JUMP " + to_string(l1 + 14) + "\n";
				//expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l1 + 46) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is positive, right side is negative
				//18 / -5
				int l2 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l2 + 10) + "\n";
				expr += "JNEG " + to_string(l2 + 10) + "\n";
				expr += "ADD " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l2 + 1) + "\n";

				//expr += "SUB " + to_string(freeCell) + "\n";
				//expr += "INC\n";
				//expr += "JUMP " + to_string(l2 + 14) + "\n";
				//expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l2 + 34) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is negative, right side is positive
				//-18 / 5
				int l3 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l3 + 10) + "\n";
				expr += "JPOS " + to_string(l3 + 10) + "\n";
				expr += "ADD " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "DEC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l3 + 1) + "\n";

				//expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l3 + 23) + "\n"; //skip other variants
				writeToFile(expr);

				//left side is negative, right side is negative
				//-18 / -5
				int l4 = generatedLines;
				expr = "LOAD 1\n";
				expr += "JZERO " + to_string(l4 + 11) + "\n";
				expr += "JPOS " + to_string(l4 + 10) + "\n";
				expr += "SUB " + to_string(freeCell) + "\n";
				expr += "STORE 1\n";
				expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "INC\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD 1\n";
				expr += "JUMP " + to_string(l4 + 1) + "\n";

				expr += "ADD " + to_string(freeCell) + "\n";
				//expr += "DEC\n";
				//expr += "JUMP " + to_string(l4 + 14) + "\n";
				//expr += "LOAD " + to_string(freeCell + 1) + "\n";
				expr += "JUMP " + to_string(l4 + 12) + "\n";
				writeToFile(expr);
			}
		}
		else
		{
			string expr2 = "STORE " + to_string(freeCell) + " #mod---------\n";
			expr2 += "SUB 0\n";
			expr2 += "INC\n";
			expr2 += "INC\n";
			expr2 += "SUB " + to_string(freeCell) + "\n";
			expr2 += "JNEG " + to_string(generatedLines + 23) + "\n";
			expr2 += "JPOS " + to_string(generatedLines + 23) + "\n";
			expr2 += "SUB 0\n";
			expr2 += "DEC\n";
			expr2 += "STORE " + to_string(freeCell + 1) + "\n";
			expr2 += "SUB 0\n";
			expr2 += "INC\n";
			expr2 += "STORE " + to_string(freeCell + 2) + "\n";
			expr2 += "LOAD 1\n";
			expr2 += "SHIFT " + to_string(freeCell + 1) + "\n";
			expr2 += "SHIFT " + to_string(freeCell + 2) + "\n";
			expr2 += "SUB 1\n";
			expr2 += "JZERO " + to_string(generatedLines + 21) + "\n";
			expr2 += "STORE 1\n";

			expr2 += "LOAD " + to_string(freeCell + 2) + "\n";
			expr2 += "JUMP " + to_string(generatedLines + 22) + "\n";
			expr2 += "STORE 1\n";
			writeToFile(expr2);
			//expr2 = "LOAD " + to_string(freeCell) + "\n";
			expr2 = "JUMP " + to_string(generatedLines + 46 + 15) + "\n";
			expr2 += "LOAD " + to_string(freeCell) + "\n";
			writeToFile(expr2);

			//decision tree
			string expr = "STORE " + to_string(freeCell) + "\n";
			expr += "SUB 0\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD " + to_string(freeCell) + "\n";
			expr += "JZERO " + to_string(generatedLines + 59) + "\n"; //div by 0

			expr += "JNEG " + to_string(generatedLines + 7) + "\n"; //right side is negative
			expr += "JUMP " + to_string(generatedLines + 10) + "\n"; //right side is positive

			expr += "LOAD 1\n"; //load left side
			expr += "JNEG " + to_string(generatedLines + 48) + "\n"; //left is neg, right is neg
			expr += "JUMP " + to_string(generatedLines + 26) + "\n"; //left is pos, right is neg

			expr += "LOAD 1\n";
			expr += "JNEG " + to_string(generatedLines + 37) + "\n"; //left is neg, right is pos
			//left is pos, right is pos	

			expr += "LOAD " + to_string(freeCell) + "\n";

			writeToFile(expr);

			//left side is positive, right side is positive
			//18 / 5
			int l1 = generatedLines;
			expr = "LOAD 1\n";
			expr += "JZERO " + to_string(l1 + 11) + "\n";
			expr += "JNEG " + to_string(l1 + 10) + "\n";
			expr += "SUB " + to_string(freeCell) + "\n";
			expr += "STORE 1\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "INC\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD 1\n";
			expr += "JUMP " + to_string(l1 + 1) + "\n";

			expr += "ADD " + to_string(freeCell) + "\n";
			//expr += "DEC\n";
			//jneg additional jump to omit load beneath
			//expr += "JUMP " + to_string(l1 + 14) + "\n";
			//expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "JUMP " + to_string(l1 + 46) + "\n"; //skip other variants
			writeToFile(expr);

			//left side is positive, right side is negative
			//18 / -5
			int l2 = generatedLines;
			expr = "LOAD 1\n";
			expr += "JZERO " + to_string(l2 + 10) + "\n";
			expr += "JNEG " + to_string(l2 + 10) + "\n";
			expr += "ADD " + to_string(freeCell) + "\n";
			expr += "STORE 1\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "DEC\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD 1\n";
			expr += "JUMP " + to_string(l2 + 1) + "\n";

			//expr += "SUB " + to_string(freeCell) + "\n";
			//expr += "INC\n";
			//expr += "JUMP " + to_string(l2 + 14) + "\n";
			//expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "JUMP " + to_string(l2 + 34) + "\n"; //skip other variants
			writeToFile(expr);

			//left side is negative, right side is positive
			//-18 / 5
			int l3 = generatedLines;
			expr = "LOAD 1\n";
			expr += "JZERO " + to_string(l3 + 10) + "\n";
			expr += "JPOS " + to_string(l3 + 10) + "\n";
			expr += "ADD " + to_string(freeCell) + "\n";
			expr += "STORE 1\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "DEC\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD 1\n";
			expr += "JUMP " + to_string(l3 + 1) + "\n";

			//expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "JUMP " + to_string(l3 + 23) + "\n"; //skip other variants
			writeToFile(expr);

			//left side is negative, right side is negative
			//-18 / -5
			int l4 = generatedLines;
			expr = "LOAD 1\n";
			expr += "JZERO " + to_string(l4 + 11) + "\n";
			expr += "JPOS " + to_string(l4 + 10) + "\n";
			expr += "SUB " + to_string(freeCell) + "\n";
			expr += "STORE 1\n";
			expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "INC\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD 1\n";
			expr += "JUMP " + to_string(l4 + 1) + "\n";

			expr += "ADD " + to_string(freeCell) + "\n";
			//expr += "DEC\n";
			//expr += "JUMP " + to_string(l4 + 14) + "\n";
			//expr += "LOAD " + to_string(freeCell + 1) + "\n";
			expr += "JUMP " + to_string(l4 + 12) + "\n";
			writeToFile(expr);
		}
	}
	catch(UndeclaredVariable undeclaredVariable)
	{
		cout << undeclaredVariable.message << endl;
	}
	catch(BadIndex badIndex)
	{
		cout << badIndex.message << endl;
	}
	catch(UninitializedVariable unitializedVariable)
	{
		cout << unitializedVariable.message << endl;
	}
}

//private
void Compiler::writeToFile(string text)
{
	for(int i = 0; i < text.length(); i++)
	{
		if(text[i] == '\n')
		{
			generatedLines++;
		}
	}

	char cString[text.size() + 1];
	strcpy(cString, text.c_str());
	fputs(cString, output);
}
