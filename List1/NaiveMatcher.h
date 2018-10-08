#include <iostream>
using namespace std;

class NaiveMatcher
{
public:
	NaiveMatcher(string, string);
	void Match();

private:
	string text;
	string pattern;
	unsigned int textLength;
	unsigned int patternLength;

	bool MatchWithOffset(int);
};

NaiveMatcher::NaiveMatcher(string textInput, string patternInput)
{
	text = textInput;
	pattern = patternInput;
	textLength = text.length();
	patternLength = pattern.length();
}

void NaiveMatcher::Match()
{
	for(unsigned int i = 0; i <= textLength - patternLength; i++)
	{
		if(MatchWithOffset(i) == true)
			cout << i << endl;
	}
}

bool NaiveMatcher::MatchWithOffset(int offset)
{
	for(unsigned int i = 0; i < patternLength; i++)
	{
		if(pattern[i] != text[offset + i])
			return false;
	}
	return true;
}
