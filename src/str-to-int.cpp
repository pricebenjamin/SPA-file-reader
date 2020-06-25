#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath>

using namespace std;

// Convert characters '0','1',...,'9' to their integer value
int charToInt(char a)
{
	int integer = 0;
	switch(a)
	{
		case '0': break; // integer is defined as 0
		case '1': integer = 1; break;
		case '2': integer = 2; break;
		case '3': integer = 3; break;
		case '4': integer = 4; break;
		case '5': integer = 5; break;
		case '6': integer = 6; break;
		case '7': integer = 7; break;
		case '8': integer = 8; break;
		case '9': integer = 9; break;
		default: {
			cerr << "Error: charToInt(char): argument was not a decimal integer." << endl;
			exit(1);
		}
	}
	return integer;
}

//
string truncateStrAt(string str, char truncAt)
{
	int length = str.length();
	for(int i = 0; i < length; i++)
		if(str[i] == truncAt) return str.substr(0,i);
	// If we're here, truncAt was not found in string
	return str;
}

string getStrAfter(string str, char dividingChar)
{
    string remainder = "***NullString***";
    bool dividingCharFound = false;
    int length = str.length();
    for(int i = 0; i < length; i++)
    {
        if(str[i] == dividingChar)
        {
            dividingCharFound = true;
            if(i == length - 1) 
            {
                remainder = ""; // dividingChar is the end of the string
                cerr << "Warning: getStrAfter(string, char): returning empty string." << endl;
            }
            else remainder = str.substr(i+1); // Read str from index i+1 to the end
            break;
        }
    }

    if(dividingCharFound) return remainder;
    else
    {
        cerr << "Error: getStrAfter(string, char): character '" << dividingChar << "' not found in string '" << str <<"'." << endl;
        exit(1);
    }
}

//
int strToInt(string numberAsString)
{
	int integer = 0;
	// Check for decimal point and truncate
	char decimalPoint = '.';
	numberAsString = truncateStrAt(numberAsString, decimalPoint);
	int length = numberAsString.length();
	// Check for negative
	if(numberAsString[0] == '-')
	{
		string absVal = numberAsString.substr(1); // Read from index 1 to the end
		return -1 * strToInt(absVal);
	} else {
		int power = 0;
		for(int i = length - 1; i >= 0; i--) // Start at the end of the string and read backwards
		{
			integer += charToInt(numberAsString[i]) * pow(10, power);
			power++;
		}
	}
	return integer;
}