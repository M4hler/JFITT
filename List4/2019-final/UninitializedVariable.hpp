#include <string>
using namespace std;

class UninitializedVariable : public exception
{
public:
	string message;

	UninitializedVariable(string);
};
