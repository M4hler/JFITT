#include <iostream>
#include "NaiveMatcher.h"
#include "DFAMatcher.h"
using namespace std;

int main()
{
	string t = "ABABABACABA";
	string p = "ABABACA";

	cout << t << endl;
//	cout << "Give pattern: ";
//	cin >> p;

//	NaiveMatcher * nm = new NaiveMatcher(t, p);
//	nm -> Match();

	DFAMatcher * dfam = new DFAMatcher(t, p);
//	dfam -> STF();
	dfam -> Match();
}
