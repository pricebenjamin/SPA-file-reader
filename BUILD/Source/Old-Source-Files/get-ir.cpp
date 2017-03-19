#include <iostream>  	// Print messages to the terminal
#include <fstream>		// Open, read, create, and edit files
#include <cstdlib>

using namespace std;

/* Program name: get-ir
 *  Usage: ./get-ir <SPA filename> [lb=<lower bound>] [ub=<upper bound>]
 *
 *  This program reads % transmittance values from <SPA filename> and saves them in <SPA filename>.csv, 
 *	containing the (estimated) wavenumber and the (hopefully) corresponding % transmittance. Additionally, 
 *	the program will append the minimum % transmittance on the given (or default) interval to a file called
 *	'minValues.csv'.
 */

/* TODO: 
 *	1) Accept 'lb' and 'ub' arguments
 *		a) Wavenumber / Index type flags
 *	2) Accept a list of SPA filenames
 *		a) Encapsulate file processing in a function
 */

/*	char* addr(void p*){
 * 		return reinterpret_cast<char*>(p);
 *	}
 */

int main(int argc, char* argv[]) {	// argc = (number of command-line arguments passed to this program)
									// argv = (vector of pointers to the command-line arguments)
    // Check for SPA filename:
    if(argc == 1) {	// If no command-line arguments were given...
        cerr << "Usage: " << argv[0] << " <SPA filename>" << endl;	// Print out the usage
        															// argv[0] is the name of this program
        return 1;	// An error occured
    }

    string SPA_FILENAME = argv[1];

    const int HEX_SEEK = 0x49C;		// Hex address of first datum in SPA file
    const int SIZE = 55587;			// Number of data in SPA file
    const float MAX_WAVENUMBER =  3999.9912;	// inverse cm; spectrometer measures from ~600 cm^-1 to ~4000 cm^-1
    const float STEP_SIZE = 0.060266;			// inverse cm; distance between sequential data
    float floatData[SIZE];		// Stores the data being read
    int ADJ_MIN = 36172;		// "Adjusted minimum" and
    int ADJ_MAX = 37501;		// "Adjusted maximum" are index values that define the region of interest
    string MIN_VAL_FILENAME = "minValues.csv"; // Name of file that will store minimum values found in the region of interest

    // Initialize the floatData vector to zeros
    for(int a = 0; a < SIZE; a++) floatData[a] = 0;
    
    // Open SPA file
    ifstream ifs (SPA_FILENAME.c_str(), ios::in|ios::binary);
    if(ifs.is_open()) {
        ifs.seekg(HEX_SEEK, ios::beg); // Go to first datum

        ifs.read(reinterpret_cast<char*>(floatData), sizeof(floatData)); // Read data into floatData
        ifs.close();

        // Output region of interest into a .csv file
        ofstream ofs (SPA_FILENAME.append(".csv").c_str(), ios::out);
        for(int a = ADJ_MIN; a < ADJ_MAX; a++) {
            ofs << MAX_WAVENUMBER - STEP_SIZE*a << ", " << floatData[a] << endl;
        }

        ofs.close();

        // Find extreme values on region of interest
        int maxIndex = ADJ_MIN;
        float max = floatData[ADJ_MIN];
        int minIndex = ADJ_MIN;
        float min = floatData[ADJ_MIN];
        for(int a = ADJ_MIN; a < ADJ_MAX; a++) {
            if(max < floatData[a]) {
                max = floatData[a];
                maxIndex = a;
            }
            if(min > floatData[a]) {
                min = floatData[a];
                minIndex = a;
            }
        }
        cout << "Max: " << max << " at " << maxIndex << endl;
        cout << "Min: " << min << " at " << minIndex << endl;

        // Save minimum values to a .csv file
        ofstream minVals (MIN_VAL_FILENAME.c_str(), ios::app);
        minVals << SPA_FILENAME.c_str() << ", " << MAX_WAVENUMBER - STEP_SIZE*minIndex << ", " << min << endl;
        minVals.close();

    } else {
        cout << "Unable to open input file." << endl;
    }

    return 0;
}
