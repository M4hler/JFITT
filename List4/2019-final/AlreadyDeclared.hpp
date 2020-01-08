#include <string>
using namespace std;

class AlreadyDeclared : public exception
{
public:
	string message;

	AlreadyDeclared(string, int);
};

AlreadyDeclared::AlreadyDeclared(string identifier, int lineNumber)
{
	message = "Variable \"" + identifier + "\" has already been declared at line " 
			+ to_string(lineNumber);
}
