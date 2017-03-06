#include "header.h"

// Ensure that ub > lb, ub <= MAX_WAVENUMBER, lb >= MIN_WAVENUMBER, etc...
void checkBounds(int* upperBound, int* lowerBound, int max, int min)
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
	if(*upperBound > max && *lowerBound < min) {
		cerr << "Error: checkBounds(int,int): upperbound > MAX_WAVENUMBER and lowerbound < MIN_WAVENUMBER." << endl;
		exit(1);
	} else if(*upperBound > max) {
		cerr << "Error: checkBounds(int,int): upperbound > MAX_WAVENUMBER." << endl;
		exit(1);
	} else if(*lowerBound < min) {
		cerr << "Error: checkBounds(int,int): lowerbound < MIN_WAVENUMBER." << endl;
		exit(1);
	}
}

void checkIfNull(void* pointer, const char* callingFunc, const char* ptrDef)
{
	if(pointer == nullptr)
	{
		cerr << "Error: " << callingFunc << ": unable to allocate memory for " << ptrDef << ".\n";
		exit(1);
	}
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
void readSPAFile(char* SPA_FILENAME, float IR_Data[], int length, int start)
{
    if(canOpenFile(SPA_FILENAME))
    {
	    ifstream spaInputFile (SPA_FILENAME, ios::in|ios::binary);
	    // Go to first datum, then read data into IR_Data
	    spaInputFile.seekg(start, ios::beg);
	    spaInputFile.read(reinterpret_cast<char*>(IR_Data), length*4); // 4 bytes per datum
	    // We must reinterpret IR_Data as a character pointer in order to correctly read in the data
	    spaInputFile.close();
	} else {
		cerr << "Error: Unable to open " << SPA_FILENAME << "." << endl;
		exit(1);
	}
	return;
}

// Append str to SPA_FILENAME
// Made const in order to fix "invalid conversion from 'const char*' to 'char*' [-fpermissive]" error
const char* appendStr(char* SPA_FILENAME, const char* str)
{
	string spaString = SPA_FILENAME;
	return spaString.append(str).c_str();
}

// Convert a wavenumber to nearest index
int wavenumToIndex(int wavenumber, float wavenumberArray[], int size)
{
	//cout << "wavenumToIndex(int): passed argument 'wavenumber' = " << wavenumber << "." << endl;
	int candidateIndex = 0;
    // Find the value in wavenumberArray that is closest to wavenumber
    for(int i = 0; i < size; i++)
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

void printToCSV(const char* CSV_FILENAME, float IR_Data[], float wavenumber[], int length, int upperBound, int lowerBound)
{
	int lowerIndex = wavenumToIndex(upperBound, wavenumber, length); // Woops...that's confusing
	int upperIndex = wavenumToIndex(lowerBound, wavenumber, length); // Higher wavenumbers have lower indices! That's how the data is ordered.
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
	int lowerIndex = wavenumToIndex(upperBound, wavenumber, length);
	int upperIndex = wavenumToIndex(lowerBound, wavenumber, length);
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
