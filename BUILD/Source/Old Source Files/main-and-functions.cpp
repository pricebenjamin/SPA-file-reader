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
void printToCSV(const char* CSV_FILENAME, char** SPA_FILENAME, float** IR_Data, float wavenumber[], int NUM_SPA_FILES, int length, int ub, int lb);
void printToCSV(const char* CSV_FILENAME, float** IR_Data, float wavenumber[], int NUM_SPA_FILES, int length, int bound, bool isLowerBound);

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
	// Check for obviously incorrect usage
	if( argc == 1 || ( argc == 2 && argv[1][0] == '-' ) ) // NOTE: this will also catch filenames that start with a '-'
	{
		printUsage(argv[0]);
		return 1;
	}

	//float IR_Data[SIZE];	// Array to store % transmission values from input file
	float wavenumber[SIZE]; // Array to store corresponding wavenumber (assumed to be the same for all input files)
	for(int i = 0; i < SIZE; i++)
		wavenumber[i] = MAX_WAVENUMBER - (STEP_SIZE * i); // Initialize wavenumber[]

	bool lowerBoundSpecified = false;
	bool upperBoundSpecified = false;
	for(int i = 0; i < argc; i++)
	{
		string arg = argv[i];
		if(arg == "-u" || arg == "--upper-bound")
		{
			cout << "Found flag -u" << endl;
			upperBoundSpecified = true;
		}
		else if(arg == "-l" || arg == "--lower-bound")
		{
			cout << "Found flag -l" << endl;
			lowerBoundSpecified = true;
		}
	}

	if(lowerBoundSpecified && upperBoundSpecified)
	{ // Both bounds were given
		//cout << "lb specified and ub specified" << endl;
		if(argc < 5) // Not enough arguments
		{
			cerr << "Error: main(): bounds were not specified." << endl;
			printUsage(argv[0]);
			return 1;
		}
		else
		{ // Attempt to parse
			// Usage: PROG_NAME -u <number> -l <number> <SPA files...>
			string lowerBoundStr = "NaN";
			string upperBoundStr = "NaN";
			string arg_1 = argv[1];
			string arg_3 = argv[3];
			if( (arg_1 == "-u" || arg_1 == "--upper-bound") && (arg_3 == "-l" || arg_3 == "--lower-bound") )
			{
				lowerBoundStr = argv[4];
				upperBoundStr = argv[2];
			}
			else if( (arg_1 == "-l" || arg_1 == "--lower-bound") && (arg_3 == "-u" || arg_3 == "--upper-bound") )
			{
				lowerBoundStr = argv[2];
				upperBoundStr = argv[4];
			}
			else
			{
				printUsage(argv[0]);
				return 1;
			}
			int lowerBound = strToInt(lowerBoundStr);
			int upperBound = strToInt(upperBoundStr);
			checkBounds(&upperBound, &lowerBound);
			// Remainder of arguments are assumed to be SPA files
			if(argc == 5)
			{
				cerr << "Error: main(): no SPA file specified." << endl;
				printUsage(argv[0]);
				return 1;
			}
			int NUM_SPA_FILES = argc - 5;

			//char* SPA_FILENAME[NUM_SPA_FILES];
			char** SPA_FILENAME;
			SPA_FILENAME = new (nothrow) char* [NUM_SPA_FILES];
			if(SPA_FILENAME == nullptr)
			{
				cerr << "Error: main(): unable to allocate memory for char** SPA_FILENAME." << endl;
				return 1;
			}
			cout << "Successfully allocated memory for SPA_FILENAME->char*[NUM_SPA_FILES]." << endl;
			for(int i = 0; i < NUM_SPA_FILES; i++)
				SPA_FILENAME[i] = argv[5 + i]; // Pretty sure this notation is good
			//cout << "Successfully stored SPA filenames." << endl;

			//float IR_Data[NUM_SPA_FILES][SIZE]; // Array of IR data
			float** IR_Data;
			IR_Data = new (nothrow) float* [NUM_SPA_FILES];
			if(IR_Data == nullptr)
			{
				delete[] SPA_FILENAME;
				cerr << "Error: main(): unable to allocate memory for float* IR_Data." << endl;
				return 1;
			}
			//cout << "Successfully allocated memory for IR_Data->float*[NUM_SPA_FILES]." << endl;
			for(int i = 0; i < NUM_SPA_FILES; i++)
			{
				IR_Data[i] = new (nothrow) float [SIZE];
				if(IR_Data[i] == nullptr)
				{
					delete[] SPA_FILENAME;
					delete[] IR_Data;
					cerr << "Error: main(): unable to allocate memory for IR_Data[" << i << "] = new float [SIZE]." << endl;
					return 1;
				}
				//cout << "Successfully allocated memory for IR_Data[" << i << "]->float[SIZE]." << endl;
			}

			for(int i = 0; i < NUM_SPA_FILES; i++)
				readSPAFile(SPA_FILENAME[i], IR_Data[i], SIZE);
			//cout << "Successfully read all SPA files." << endl;

			string filename = "combinedData";
			string ubStr = to_string(upperBound);
			string lbStr = to_string(lowerBound);
			const char* CSV_FILENAME = filename.append(".").append(ubStr).append("-").append(lbStr).append(".CSV").c_str();
			printToCSV(CSV_FILENAME, SPA_FILENAME, IR_Data, wavenumber, NUM_SPA_FILES, SIZE, upperBound, lowerBound);

			delete[] SPA_FILENAME;
			for(int i = 0; i < NUM_SPA_FILES; i++)
				delete[] IR_Data[i];
			delete[] IR_Data;
		}
	} 
	else if(lowerBoundSpecified || upperBoundSpecified)
	{ // One bound given

	} 
	else
	{ // No bounds given: assume all arguments are SPA files
		cout << "I didn't see you pass me any bounds..." << endl;
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

void printToCSV
(
	const char* CSV_FILENAME,
	char** SPA_FILENAME,
	float** IR_Data, 
	float wavenumber[], 
	int NUM_SPA_FILES, 
	int length, 
	int upperBound, 
	int lowerBound
)
{
	int lowerIndex = wavenumToIndex(upperBound, wavenumber);
	int upperIndex = wavenumToIndex(lowerBound, wavenumber);
	ofstream csvOutputFile (CSV_FILENAME, ios::out);
	if(csvOutputFile.is_open())
	{
		// Headings
		csvOutputFile << "Wavenumber, ";
		for(int i = 0; i < NUM_SPA_FILES - 1; i++)
			csvOutputFile << SPA_FILENAME[i] << ", ";
		csvOutputFile << SPA_FILENAME[NUM_SPA_FILES - 1] << endl;

		// Data
		for(int i = lowerIndex; i < upperIndex + 1; i++)
		{
			csvOutputFile << wavenumber[i] << ", ";
			for(int j = 0; j < NUM_SPA_FILES - 1; j++)
				csvOutputFile << IR_Data[j][i] << ", ";
			csvOutputFile << IR_Data[NUM_SPA_FILES - 1][i] << endl;
		}
		csvOutputFile.close();
	}
	else
	{
		cerr << "Error: printToCSV(const char*, char**, float**, float[], int, int, int, int): unable to open output file." << endl;
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
