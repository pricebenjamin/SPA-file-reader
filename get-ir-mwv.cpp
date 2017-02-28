#include <iostream>  	// Print messages to the terminal
#include <fstream>		// Open, read, create, and edit files
#include <cstdlib>

using namespace std;

/* Program name: get-ir
 *  Usage: ./get-ir <SPA filename>
 *
 *  This program reads % transmittance values from <SPA filename> and saves them in <SPA filename>.csv, 
 *	containing the (estimated) wavenumber and the (hopefully) corresponding % transmittance.
 */

// Function prototypes:
void printUsage(char* PROG_NAME);
bool canOpenFile(char* FILENAME);
void readSPAFile(char* SPA_FILENAME, float IR_Data[], int length);
const char* appendCSV(char* SPA_FILENAME);
void printToCSV(const char* CSV_FILENAME, float IR_Data[], int length); // No idea why CSV_FILENAME needed to be const...

// Global constants: (these depend on my personal SPA files)
const int HEX_START = 0x49C;		// Hex address just before first byte of first datum in SPA file
const int HEX_END = 0x036929;		// Hex address of last byte of last datum
const int SIZE = (HEX_END - HEX_START) / 4;	// Number of data in SPA file (4 bytes per datum)
const float MAX_WAVENUMBER =  3999.9907;	// inverse cm
const float MIN_WAVENUMBER = 649.9812;		// inverse cm
const float STEP_SIZE = (MAX_WAVENUMBER - MIN_WAVENUMBER) / (SIZE - 1);	// inverse cm; distance between sequential data

int main(int argc, char* argv[]) {	
	// argc = (number of command-line arguments passed to this program)
	// argv = (array of pointers to the command-line arguments)
	switch(argc) {
		case 2: { // Changed scope to fix "crosses initialization of 'char* SPA_FILENAME'" error
				char* SPA_FILENAME = argv[1];
				if(canOpenFile(SPA_FILENAME)) {
					float IR_Data[SIZE];
					// for(int i = 0; i < SIZE; i++) IR_Data[i] = 0; // Initialize array with zeros
					readSPAFile(SPA_FILENAME, IR_Data, SIZE);
					printToCSV(appendCSV(SPA_FILENAME), IR_Data, SIZE);
				} else {
					cerr << "Unable to open " << SPA_FILENAME << "." << endl;
					return 1;
				}
			} break;
		default:
			printUsage(argv[0]); // argv[0] is the name of the compiled program when executed from command-line
			return 1;
	}
    return 0;
}

// Print the correct usage of this program
void printUsage(char* PROG_NAME) {
	cerr << "Usage: " << PROG_NAME << " <SPA filename>" << endl;	// Print out the usage
	return;
}

// Verify that the SPA file can be opened
bool canOpenFile(char* FILENAME){
	ifstream file (FILENAME, ios::in|ios::binary);
	bool ableToOpen = file.is_open();
	if(ableToOpen) file.close();
	return ableToOpen;
}

// Read the contents of the SPA file into an array
void readSPAFile(char* SPA_FILENAME, float IR_Data[], int length) {
    // Open SPA file
    // Remember: we already ensured that this file can be opened in main()
    ifstream spaInputFile (SPA_FILENAME, ios::in|ios::binary);
    // Go to first datum, then read data into IR_Data
    spaInputFile.seekg(HEX_START, ios::beg);
    spaInputFile.read(reinterpret_cast<char*>(IR_Data), length*4); // 4 bytes per datum
    // We must reinterpret IR_Data as a character pointer in order to correctly read in the data
    spaInputFile.close();
    return;
}

// Append ".CSV" to SPA_FILENAME
// Made const in order to fix "invalid conversion from 'const char*' to 'char*' [-fpermissive]" error
const char* appendCSV(char* SPA_FILENAME) {
	string spaString = SPA_FILENAME;
	return spaString.append(".CSV").c_str();
}

// Print array to CSV file
void printToCSV(const char* CSV_FILENAME, float IR_Data[], int length) {
	// Output IR_Data to a CSV file
    ofstream csvOutputFile (CSV_FILENAME, ios::out);
    if(csvOutputFile.is_open()){
	    for(int a = 0; a < length; a++) {
	        csvOutputFile << MAX_WAVENUMBER - STEP_SIZE*a << ", " << IR_Data[a] << endl;
	    }
	    csvOutputFile.close();
	} else {
		cerr << "Unable to open output file." << endl;
	}
}
