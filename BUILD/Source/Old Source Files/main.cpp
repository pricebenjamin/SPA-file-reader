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
void printUsage(char* PROG_NAME)
{
	cerr << "Usage: " << PROG_NAME << "  [-u <upper wavenumber>] [-l <lower wavenumber>] <SPA filename> [<SPA filename 2> ... ]\n";
	cerr << "    [ (-u | --upper-bound) <number> ]: <number> is an integer which specifies the upper bound of the interval to be exported to a CSV file\n";
	cerr << "    [ (-l | --lower-bound) <number> ]: <number> is an integer which specifies the lower bound of the interval to be exported to a CSV file\n";
	return;
}
bool canOpenFile(char* FILENAME);
void readSPAFile(char* SPA_FILENAME, float IR_Data[], int length, int start);
const char* appendStr(char* SPA_FILENAME, const char* str);
int charToInt(char);
string truncateStrAt(string str, char truncAt);
int strToInt(string numberAsString);
void checkBounds(int* upperBound, int* lowerBound, int max, int min);
void checkIfNull(void* pointer, const char* callingFunc, const char* ptrDef);
int wavenumToIndex(int wavenumber, float wavenumberArray[], int size);
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

	// Check for boundary flags in command-line arguments
	bool lowerBoundSpecified = false;
	bool upperBoundSpecified = false;
	for(int i = 0; i < argc; i++)
	{
		string arg = argv[i];
		if(arg == "-u" || arg == "--upper-bound")
		{
			//cout << "Found flag -u" << endl;
			upperBoundSpecified = true;
		}
		else if(arg == "-l" || arg == "--lower-bound")
		{
			//cout << "Found flag -l" << endl;
			lowerBoundSpecified = true;
		}
	}

	// Begin parsing command-line arguments based on flags
	if(lowerBoundSpecified && upperBoundSpecified)
	{ // ======================================== BEGIN SCENARIO: both bounds given ===================================
		//cout << "Info: main(): lowerBoundSpecified && upperBoundSpecified == true." << endl;
		if(argc < 5) // Not enough arguments: flags were given, but values were not provided
		{
			cerr << "Error: main(): bounds were not specified." << endl;
			printUsage(argv[0]);
			return 1;
		}
		else
		{ // Bounds were given, and enough arguments were provided: attempt to parse arguments
			// Usage: PROG_NAME -u <number> -l <number> <SPA files...>

			// ========================= Locate, parse, and check the upper and lower bounds ==========================
			// NOTE: the flags and values are expected to be located in argv[1-4]
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
			checkBounds(&upperBound, &lowerBound, MAX_WAVENUMBER, MIN_WAVENUMBER);

			// ========================================= Check for SPA files ==========================================
			// NOTE: remainder of arguments are assumed to be SPA files
			if(argc == 5)
			{
				cerr << "Error: main(): no SPA file specified." << endl;
				printUsage(argv[0]);
				return 1;
			}
			// ==================================== Prepare to read data from files ===================================
			int NUM_SPA_FILES = argc - 5;
			// -------------------------------------- Create vector of filenames --------------------------------------
			char** SPA_FILENAME;
			SPA_FILENAME = new (nothrow) char* [NUM_SPA_FILES];
			if(SPA_FILENAME == nullptr)
			{
				cerr << "Error: main(): unable to allocate memory for char** SPA_FILENAME." << endl;
				return 1;
			}
			// -------------------------------------- Initialize filename vector --------------------------------------
			for(int i = 0; i < NUM_SPA_FILES; i++)
				SPA_FILENAME[i] = argv[5 + i]; // Remember: argv also contains PROG_NAME and flags; SPA filenames start at argv[5]

			// ------------------------------------ Create array to store IR data -------------------------------------
			float** IR_Data;
			IR_Data = new (nothrow) float* [NUM_SPA_FILES];
			if(IR_Data == nullptr)
			{
				delete[] SPA_FILENAME;
				cerr << "Error: main(): unable to allocate memory for float* IR_Data." << endl;
				return 1;
			}
			// --------------------------------- Initialize first layer of the array ----------------------------------
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
			}
			// ------------------------------------- Read data into IR data Array -------------------------------------
			for(int i = 0; i < NUM_SPA_FILES; i++)
				readSPAFile(SPA_FILENAME[i], IR_Data[i], SIZE, HEX_START);

			// ==================================== Prepare to write data to output ===================================
			// ------------------------------------- Creating name for output file ------------------------------------
			string filename = "combinedData";
			string ubStr = to_string(upperBound);
			string lbStr = to_string(lowerBound);
			const char* CSV_FILENAME = filename.append(".").append(ubStr).append("-").append(lbStr).append(".CSV").c_str();
			// ---------------------------------------- Print data to output file -------------------------------------
			printToCSV(CSV_FILENAME, SPA_FILENAME, IR_Data, wavenumber, NUM_SPA_FILES, SIZE, upperBound, lowerBound);

			// =========================================== Data processing ============================================
			// NOTE: This section is being created on-the-fly in order to finish a report. As such, very little is done
			//		 to ensure that command-line arguments are in the required order. Care must be taken when executing
			//		 the program in order to obtain meaningful data. Specifically,
			//		 		(!!) data files are assumed to be ordered into groups of three. 
			//		 The expected syntax is as follows:
			//				./PROG_NAME -u <bound> -l <bound> <SPA file 11> <SPA file 12> <SPA file 13>
			//												  <SPA fiel 21> <SPA file 22> <SPA file 23> ...
			if(NUM_SPA_FILES % 3 == 0) // Three IR spectra were taken for each time sample
			{ // This is not a very unique check, but it's the best I can do at the moment.
				int numCols = NUM_SPA_FILES / 3;
				// Create array to store the average % transmission of the 3 spectra at each wavelength
				float** averageValues;
				averageValues = new (nothrow) float* [numCols];
				checkIfNull(averageValues, "main()", "float** averageValues");
				for(int i = 0; i < numCols; i++)
				{
					averageValues[i] = new (nothrow) float [SIZE];
					string str = "averageValues";
					const char* ptrDef = str.append("[").append(to_string(i)).append("]").c_str();
					checkIfNull(averageValues[i], "main()", ptrDef);
				}
				// Calculate and store average values
				for(int i = 0; i < numCols; i++)
					for(int j = 0; j < SIZE; j++)
					{
						float average = ( IR_Data[3*i][j] + IR_Data[3*i + 1][j] + IR_Data[3*i + 2][j] ) / 3.0;
						averageValues[i][j] = average;
					}

				// Print averageValues to file
				char** columnTitles;
				columnTitles = new (nothrow) char* [numCols];
				checkIfNull(columnTitles, "main()", "char** columnTitles");
				for(int i = 0; i < numCols; i++)
					columnTitles[i] = SPA_FILENAME[3*i];

				string filename = "averageValues";
				const char* avgValFile = filename.append(".").append(ubStr).append("-").append(lbStr).append(".CSV").c_str();
				printToCSV(avgValFile, columnTitles, averageValues, wavenumber, numCols, SIZE, upperBound, lowerBound);

				delete[] columnTitles;
				for(int i = 0; i < numCols; i++)
					delete[] averageValues[i];
				delete[] averageValues;
			}
			// ================================== Delete dynamically allocated memory =================================
			delete[] SPA_FILENAME;
			for(int i = 0; i < NUM_SPA_FILES; i++)
				delete[] IR_Data[i];
			delete[] IR_Data;
		}
	} // ========================================= END SCENARIO: both bounds given ====================================
	else if(lowerBoundSpecified || upperBoundSpecified)
	{ // ========================================= BEGIN SCENARIO: one bound given ====================================
		if(lowerBoundSpecified)
		{
			cout << "Info: lowerBound was specified; assuming desired interval from lowerBound to MAX_WAVENUMBER.\n";
		}
		else
		{
			cout << "Info: upperBound was specified; assuming desired interval from MIN_WAVENUMBER to upperBound.\n";
		}
		cerr << "Error: main(): one bound scenario is still under development...\n";
		return 1;
	} // ========================================== END SCENARIO: one bound given =====================================
	else
	{ // ========================================= BEGIN SCENARIO: no bounds given ====================================
		// No bounds given: assume all arguments are SPA files
		cout << "Info: neither upperBound nor lowerBound were specified; assuming all arguments are SPA files.\n";
		cerr << "Error: main(): no bounds scenario is still under development...\n";
		return 1;
	} // ========================================== END SCENARIO: no bounds given =====================================
    return 0;
}
