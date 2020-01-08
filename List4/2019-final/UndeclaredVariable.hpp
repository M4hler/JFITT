#include <string>
using namespace std;

class UndeclaredVariable : public exception
{
public:
	string message;

	UndeclaredVariable(string);
};
