#include <iostream>  	// Print messages to the terminal
#include <fstream>		// Open, read, create, and edit files
#include <cstdlib>		// exit()
#include <string>		// to_string()
#include <cmath>		// pow(), abs()

using namespace std;

/* Program name: get-ir
 *  Usage: ./get-ir [-u <upper wavenumber> -l <lower wavenumber>] <SPA filename>
 *
 *  This program reads % transmittance values from <SPA filename> and saves them in <SPA filename>.csv, 
 *	containing the (estimated) wavenumber and the (hopefully) corresponding % transmittance.
 */

// Function prototypes:
void printUsage(char* PROG_NAME);
bool canOpenFile(char* FILENAME);
void readSPAFile(char* SPA_FILENAME, float IR_Data[], int length);
const char* appendStr(char* SPA_FILENAME, const char* str);
int charToInt(char);
string truncateStrAt(string str, char truncAt);
int strToInt(string numberAsString);
void checkBounds(int* upperBound, int* lowerBound);
int wavenumToIndex(int wavenumber, float wavenumberArray[]);
void printToCSV(const char* CSV_FILENAME, float IR_Data[], float wavenumber[], int length);
void printToCSV(const char* CSV_FILENAME, float IR_Data[], float wavenumber[], int length, int ub, int lb);

// Global constants: (these depend on my personal SPA files)
const int HEX_START = 0x49C;		// Hex address just before first byte of first datum in SPA file
const int HEX_END = 0x036929;		// Hex address of last byte of last datum
const int SIZE = (HEX_END - HEX_START) / 4;	// Number of data in SPA file (4 bytes per datum)
const float MAX_WAVENUMBER =  3999.9907;	// inverse cm
const float MIN_WAVENUMBER = 649.9812;		// inverse cm
const float STEP_SIZE = (MAX_WAVENUMBER - MIN_WAVENUMBER) / (SIZE - 1);	// inverse cm; distance between sequential data

int main(int argc, char* argv[])
{	
	// argc = (number of command-line arguments passed to this program)
	// argv = (array of pointers to the command-line arguments)
	float IR_Data[SIZE];	// Array to store % transmission values from input file
	float wavenumber[SIZE]; // Array to store corresponding wavenumber (assumed to be the same for all input files)
	for(int i = 0; i < SIZE; i++)
		wavenumber[i] = MAX_WAVENUMBER - (STEP_SIZE * i); // Initialize wavenumber[]

	switch(argc)
	{
		case 1: printUsage(argv[0]); break;
		case 2: { // Usage: PROG_NAME <SPA filename>
				// Change scope to protect variables: prevents 'crosses initialization' error
				char* SPA_FILENAME = argv[1];
				readSPAFile(SPA_FILENAME, IR_Data, SIZE);
				printToCSV(appendStr(SPA_FILENAME, ".CSV"), IR_Data, wavenumber, SIZE);
				} break;
		case 6: { // Usage: PROG_NAME -u <upper wavenumber> -l <lower wavenumber> <SPA filename>
				string upperBoundStr = argv[2]; // Need to convert from char[] to string for strToInt(string)
				string lowerBoundStr = argv[4];
				int upperBound = strToInt(upperBoundStr);
				int lowerBound = strToInt(lowerBoundStr);
				checkBounds(&upperBound, &lowerBound);
				char* SPA_FILENAME = argv[5];
				readSPAFile(SPA_FILENAME, IR_Data, SIZE);
				string ubStr = to_string(upperBound);
				string lbStr = to_string(lowerBound);
				string dot = ".";
				const char* suffix = dot.append(ubStr).append("-").append(lbStr).append(".CSV").c_str();
				printToCSV(appendStr(SPA_FILENAME, suffix), IR_Data, wavenumber, SIZE, upperBound, lowerBound);
				} break;
		default: // ./get-ir-mwv -u <upper> -l <lower> <SPA file 1> <SPA file 2> ... // TODO
			printUsage(argv[0]); // argv[0] is the name of the compiled program when executed from command-line
			return 1;
	}
    return 0;
}

// Print the correct usage of this program
void printUsage(char* PROG_NAME)
{
	cerr << "Usage: " << PROG_NAME << "  [-u <upper wavenumber>] [-l <lower wavenumber>] <SPA filename> [<SPA filename 2> ... ]\n";
	cerr << "    [ (-u | --upper-bound) <number> ]: <number> is an integer which specifies the upper bound of the interval to be exported to a CSV file\n";
	cerr << "    [ (-l | --lower-bound) <number> ]: <number> is an integer which specifies the lower bound of the interval to be exported to a CSV file\n";
	return;
}

// Verify that the SPA file can be opened
bool canOpenFile(char* FILENAME)
{
	ifstream file (FILENAME, ios::in|ios::binary);
	bool ableToOpen = file.is_open();
	if(ableToOpen) file.close();
	return ableToOpen;
}

// Read the contents of the SPA file into an array
void readSPAFile(char* SPA_FILENAME, float IR_Data[], int length)
{
    if(canOpenFile(SPA_FILENAME))
    {
	    ifstream spaInputFile (SPA_FILENAME, ios::in|ios::binary);
	    // Go to first datum, then read data into IR_Data
	    spaInputFile.seekg(HEX_START, ios::beg);
	    spaInputFile.read(reinterpret_cast<char*>(IR_Data), length*4); // 4 bytes per datum
	    // We must reinterpret IR_Data as a character pointer in order to correctly read in the data
	    spaInputFile.close();
	} else {
		cerr << "Error: Unable to open " << SPA_FILENAME << "." << endl;
		exit(1);
	}
	return;
}

// Append ".CSV" to SPA_FILENAME
// Made const in order to fix "invalid conversion from 'const char*' to 'char*' [-fpermissive]" error
const char* appendStr(char* SPA_FILENAME, const char* str)
{
	string spaString = SPA_FILENAME;
	return spaString.append(str).c_str();
}

// Print array to CSV file
void printToCSV(const char* CSV_FILENAME, float IR_Data[], float wavenumber[], int length)
{
    ofstream csvOutputFile (CSV_FILENAME, ios::out);
    if(csvOutputFile.is_open()){
	    for(int a = 0; a < length; a++) {
	        csvOutputFile << wavenumber[a] << ", " << IR_Data[a] << endl;
	    }
	    csvOutputFile.close();
	} else {
		cerr << "Error: printToCSV(const char*, float[], float[], int): Unable to open output file " << CSV_FILENAME << "." << endl;
		exit(1);
	}
	return;
}
// Overloading printToCSV
void printToCSV(const char* CSV_FILENAME, float IR_Data[], float wavenumber[], int length, int upperBound, int lowerBound)
{
	int lowerIndex = wavenumToIndex(upperBound, wavenumber); // Woops...that's confusing
	int upperIndex = wavenumToIndex(lowerBound, wavenumber); // Higher wavenumbers have lower indices! That's how the data is ordered.
    ofstream csvOutputFile (CSV_FILENAME, ios::out);
    if(csvOutputFile.is_open()){
	    for(int a = lowerIndex; a < upperIndex + 1; a++) {
	        csvOutputFile << wavenumber[a] << ", " << IR_Data[a] << endl;
	    }
	    csvOutputFile.close();
	} else {
		cerr << "Error: printToCSV(const char*, float[], float[], int, int, int): Unable to open output file " << CSV_FILENAME << "." << endl;
		exit(1);
	}
	return;
}

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

// Ensure that ub > lb, ub <= MAX_WAVENUMBER, lb >= MIN_WAVENUMBER, etc...
void checkBounds(int* upperBound, int* lowerBound)
{
	if(*upperBound == *lowerBound) // Dereference and compare
	{
		cerr << "Error: checkBounds(int,int): upperbound equals lowerbound." << endl;
		exit(1);
	} else if(*upperBound < *lowerBound) {// Swap value stored in upperbound with value in lowerbound
		int temp = *upperBound;
		*upperBound = *lowerBound;
		*lowerBound = temp;
	}
	// Once swapped, check for invalid bounds
	if(*upperBound > MAX_WAVENUMBER && *lowerBound < MIN_WAVENUMBER) {
		cerr << "Error: checkBounds(int,int): upperbound > MAX_WAVENUMBER and lowerbound < MIN_WAVENUMBER." << endl;
		exit(1);
	} else if(*upperBound > MAX_WAVENUMBER) {
		cerr << "Error: checkBounds(int,int): upperbound > MAX_WAVENUMBER." << endl;
		exit(1);
	} else if(*lowerBound < MIN_WAVENUMBER) {
		cerr << "Error: checkBounds(int,int): lowerbound < MIN_WAVENUMBER." << endl;
		exit(1);
	}
}

// Convert a wavenumber to nearest index
int wavenumToIndex(int wavenumber, float wavenumberArray[])
{
	//cout << "wavenumToIndex(int): passed argument 'wavenumber' = " << wavenumber << "." << endl;
	int candidateIndex = 0;
    // Find the value in wavenumberArray that is closest to wavenumber
    for(int i = 0; i < SIZE; i++)
    {
    	if(wavenumber < wavenumberArray[i]) 
    		candidateIndex = i;
    	else 
    		break;
    } // candidateIndex now stores the index of the wavenumberArray value which is JUST GREATER THAN wavenumber
    float leftDistance = abs(wavenumber - wavenumberArray[candidateIndex]);
    float rightDistance = abs(wavenumber - wavenumberArray[candidateIndex + 1]);
    if(leftDistance == rightDistance || leftDistance < rightDistance)
    {
    	//cout << "Found candidateIndex: " << candidateIndex << " with corresponding value " << wavenumberArray[candidateIndex] << "." << endl;
    	return candidateIndex;
    }
    else
    {
    	//cout << "Found candidateIndex: " << candidateIndex + 1 << " with corresponding value " << wavenumberArray[candidateIndex + 1] << "." << endl;
    	return candidateIndex + 1; 	// Note: candidateIndex cannot ever be the last index;
    								// this is prevented by truncation and checkBounds()
    }
}
