#include <iostream>
using namespace std;

class KMPMatcher
{
public:
	KMPMatcher(string, string);

	void Match();
	void ShowPrefixTable();

private:
	string text;
	string pattern;
	unsigned int textLength;
	unsigned int patternLength;
	int finalState;
	int * prefixTable;

	void ComputePrefixFunction();
};

KMPMatcher::KMPMatcher(string textInput, string patternInput)
{
	text = textInput;
	pattern = patternInput;
	textLength = text.length();
	patternLength = pattern.length();
	prefixTable = new int [patternLength];

	ComputePrefixFunction();
}

void KMPMatcher::ShowPrefixTable()
{
	for(unsigned int i = 0; i < patternLength; i++)
	{
		cout << prefixTable[i] << " ";
	}
	cout << endl;
}

void KMPMatcher::Match()
{
	unsigned int q = -1;
	for(unsigned int i = 0; i < textLength; i++)
	{
		while(q > -1 && pattern[q + 1] != text[i])
		{
			q = prefixTable[q];
		}

		if(pattern[q + 1] == text[i])
			q++;

		if(q == patternLength - 1)
		{
			cout << i - patternLength + 1 << endl;
			q = prefixTable[q];
		}
	}
}

void KMPMatcher::ComputePrefixFunction()
{
	prefixTable[0] = -1;
	int k = -1;

	for(unsigned int i = 1; i < patternLength; i++)
	{
		while(k > -1 && pattern[k + 1] != pattern[i])
		{
			k = prefixTable[k];
		}

		if(pattern[k + 1] == pattern[i])
		{
			k++;
		}

		prefixTable[i] = k + 1;
	}
	prefixTable[0] = 0;
}







