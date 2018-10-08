#include <iostream>
using namespace std;

class DFAMatcher
{
public:
	DFAMatcher(string, string);

	void STF()
	{
		for(unsigned int i = 0; i <= patternLength; i++)
		{
			for(unsigned int j = 0; j < 90 - 65 + 1; j++)
			{
				cout << transitionFunction[i][j] << " ";
			}
			cout << endl;
		}
	}

	void Match()
	{
		int state = 0;
		for(unsigned int i = 0; i < textLength; i++)
		{
			state = transitionFunction[state][text[i] - 65];

			if(state == finalState)
				cout << i - finalState + 1 << endl;
		}
	}

private:
	string text;
	string pattern;
	unsigned int textLength;
	unsigned int patternLength;
	int finalState;
	int ** transitionFunction;

	void ComputeTransitionFunction(/* pattern, alfabet */)
	{
		for(unsigned int i = 0; i <= patternLength; i++)
		{
			string patternSubstring = pattern.substr(0, i);

			for(unsigned int j = 65; j <= 90; j++) //ASCII codes A-Z - alphabet
			{
				string patternSbstrWithSymbol = patternSubstring + (char)j;
				int k = min(patternLength + 1, i + 2);
				string patternKSymbolSbstr = "";

				do
				{
					k--;
					patternKSymbolSbstr = pattern.substr(0, k);
				}
				while(patternSbstrWithSymbol.rfind(patternKSymbolSbstr) != 
						patternSbstrWithSymbol.length() - patternKSymbolSbstr.length());

				transitionFunction[i][j - 65] = k;
			}
		}
	}
};

DFAMatcher::DFAMatcher(string textInput, string patternInput)
{
	text = textInput;
	pattern = patternInput;
	textLength = text.length();
	patternLength = pattern.length();
	finalState = patternLength;
	transitionFunction = new int * [patternLength + 1];

	for(unsigned int i = 0; i <= patternLength; i++)
	{
		transitionFunction[i] = new int[90 - 65 + 1]; 
	}

	ComputeTransitionFunction();
}
