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
void printToCSV(const char* CSV_FILENAME, float IR_Data[], int length); // No idea why CSV_FILENAME needed to be const...

// Global constants:
const int HEX_SEEK = 0x49C;		// Hex address of first datum in SPA file
const int SIZE = 55587;			// Number of data in SPA file
const float MAX_WAVENUMBER =  3999.9912;	// inverse cm; spectrometer measures from ~600 cm^-1 to ~4000 cm^-1
const float STEP_SIZE = 0.060266;			// inverse cm; distance between sequential data

int main(int argc, char* argv[]) {	
	// argc = (number of command-line arguments passed to this program)
	// argv = (array of pointers to the command-line arguments)
	switch(argc) {
		case 2: { // Changed scope to fix "crosses initialization of 'char* SPA_FILENAME'" error
				char* SPA_FILENAME = argv[1];
				if(canOpenFile(SPA_FILENAME)) {
					float IR_Data[SIZE];
					for(int i = 0; i < SIZE; i++) IR_Data[i] = 0; // Initialize array with zeros
					readSPAFile(SPA_FILENAME, IR_Data, SIZE);
					string spaString = SPA_FILENAME;
					const char* CSV_FILENAME = spaString.append(".csv").c_str();
					// Made const in order to fix "invalid conversion from 'const char*' to 'char*' [-fpermissive]" error
					printToCSV(CSV_FILENAME, IR_Data, SIZE);
				} else {
					cerr << "Unable to open " << SPA_FILENAME << "." << endl;
					return 1;
				}
			}
			break;
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
    spaInputFile.seekg(HEX_SEEK, ios::beg);
    spaInputFile.read(reinterpret_cast<char*>(IR_Data), length); // sizeof(IR_Data));
    // We must reinterpret IR_Data as a character pointer in order to correctly read in the data
    spaInputFile.close();
    return;
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
