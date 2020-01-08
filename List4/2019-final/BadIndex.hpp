#include <string>
using namespace std;

class BadIndex : public exception
{
public:
	string message;

	BadIndex(string, long long, long long, long long);
};
