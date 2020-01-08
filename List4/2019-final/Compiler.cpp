#include <iostream>
#include <cstring>
#include "Compiler.hpp"
#include "SymbolsTable.hpp"

#include "UndeclaredVariable.hpp"
#include "UninitializedVariable.hpp"
#include "BadIndex.hpp"
using namespace std;

extern SymbolsTable * symbolsTable;
extern long long generatedLines;

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
}

void Compiler::assign(string varIdentifier, long long value)
{

	try
	{
		symbolsTable -> checkIfDeclared(varIdentifier);

		/*if(symbolsTable -> checkIfArray(varIdentifier) == true)
		{
			if(indexIdentifier != "")
			{
				string expr = "STOREI " + to_string(storePlace) + "\n";

				indexIdentifier = "";
				writeToFile(expr);
			}
			else //array(number)
			{
				string fullName = identifier + "(" + to_string(numberIdentifier) + ")";
				symbolsTable -> initVariable(fullName);

				string expr = "STORE " + to_string(storePlace) + "\n";

				writeToFile(expr);
			}
		}
		else
		{
			symbolsTable -> initVariable(varIdentifier);

			string expr = "STORE " + to_string(storePlace) + "\n";

			writeToFile(expr);
		} */
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

				//generate number of indexes below 0
				//expr = "";
				//long long index = symbolsTable -> getArrayFirstIndex(varIdentifier);
				//if(index < 0)
				//{
				//	generateConstant(-index);
				//	expr += "STORE " + to_string(freeCell + 1) + "\n";
				//}

				//generate first available index
				expr = "";
				long long index = symbolsTable -> getArrayFirstIndex(varIdentifier);
				generateConstant(index);
				expr += "STORE " + to_string(freeCell + 1) + "\n";
								
				//load value of indexIdentifier
				long long memoryLocation = symbolsTable -> getVarMemoryLocation(indexIdentifier);
				expr += "LOAD " + to_string(memoryLocation) + "\n";

				//add everything
				//expr += "ADD " + to_string(freeCell) + "\n";

				//if(index < 0)
				//{
				//	expr += "ADD " + to_string(freeCell + 1) + "\n";
				//}

				//calculate difference between indetifier value and first available index
				expr += "SUB " + to_string(freeCell + 1) + "\n";

				//add array start memory location
				expr += "ADD " + to_string(freeCell) + "\n";

				//decrement
				//if(index > 0)
				//{
				//	expr += "DEC\n";
				//}

				//store it
				expr += "STORE " + to_string(freeCell + 2) + "\n";

				//get external value
				expr += "GET\n";

				//storei in proper place 
				expr += "STOREI " + to_string(freeCell + 2) + "\n";

				//initialize variable
				//probably not possible

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

					//generate number of indexes below 0
					//expr = "";
					//long long index = symbolsTable -> getArrayFirstIndex(identifier);
					//if(index < 0)
					//{
					//	generateConstant(-index);
					//	expr += "STORE " + to_string(freeCell + 1) + "\n";
					//}

					//generate first available index
					expr = "";
					long long index = symbolsTable -> getArrayFirstIndex(identifier);
					generateConstant(index);
					expr += "STORE " + to_string(freeCell + 1) + "\n";

					//load value of indexIdentifier
					long long memoryLocation = symbolsTable -> getVarMemoryLocation(indexIdentifier);
					expr += "LOAD " + to_string(memoryLocation) + "\n";

					//add everything
					//expr += "ADD " + to_string(freeCell) + "\n";

					//if(index < 0)
					//{
					//	expr += "ADD " + to_string(freeCell + 1) + "\n";
					//}

					//decrement
					//if(index > 0)
					//{
					//	expr += "DEC\n";
					//}

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
		string expr = "STORE " + to_string(freeCell) + "\n";
		expr += "SUB 0\n";
		expr += "STORE " + to_string(freeCell + 1) + "\n";
		expr += "LOAD " + to_string(freeCell) + "\n";
		expr += "JNEG " + to_string(generatedLines + 15) + "\n";
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

				string expr = "STORE " + to_string(freeCell) + "\n";
				expr += "SUB 0\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JNEG " + to_string(generatedLines + 15) + "\n";
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
				string expr = "STORE " + to_string(freeCell) + "\n";
				expr += "SUB 0\n";
				expr += "STORE " + to_string(freeCell + 1) + "\n";
				expr += "LOAD " + to_string(freeCell) + "\n";
				expr += "JNEG " + to_string(generatedLines + 15) + "\n";
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
			string expr = "STORE " + to_string(freeCell) + "\n";
			expr += "SUB 0\n";
			expr += "STORE " + to_string(freeCell + 1) + "\n";
			expr += "LOAD " + to_string(freeCell) + "\n";
			expr += "JNEG " + to_string(generatedLines + 15) + "\n";
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

//TODO implement
void Compiler::div()
{
	cout << "test" << endl;
}

//TODO implement - same as div, result is in different place
void Compiler::mod()
{

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
