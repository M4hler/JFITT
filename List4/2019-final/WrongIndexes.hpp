#include <string>
using namespace std;

class WrongIndexes : public exception
{
public:
	string message;

	WrongIndexes(string, long long, long long, long long);
};

WrongIndexes::WrongIndexes(string varId, long long start, long long end, long long lineNumber)
{
	message = "Array's first index must be lower or equal to second index(in \"" + varId 
			+ "\" first index is " + to_string(start) + " and second one is " + to_string(end)
			+ " at line " + to_string(lineNumber) + ")";
}
