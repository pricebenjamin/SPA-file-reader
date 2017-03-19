#include "header.h"

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
	const char* funcDef = "void readSPAFile(char*, float [], int, int)";
    if(canOpenFile(SPA_FILENAME))
    {
	    ifstream spaInputFile (SPA_FILENAME, ios::in|ios::binary);
	    // Go to first datum, then read data into IR_Data
	    spaInputFile.seekg(start, ios::beg);
	    spaInputFile.read(reinterpret_cast<char*>(IR_Data), length*4); // 4 bytes per datum
	    // We must reinterpret IR_Data as a character pointer in order to correctly read in the data
	    spaInputFile.close();
	} else {
		cerr << "Error: " << funcDef << ": Unable to open SPA file '" << SPA_FILENAME << "'." << endl;
		exit(1);
	}
	return;
}

// Print array to CSV file
// No bounds specified: print entire spectrum
void printToCSV
(
	const char* CSV_FILENAME,
	char** SPA_FILENAME,
	float** IR_Data,
	float wavenumber[],
	int NUM_SPA_FILES,
	int SIZE
)
{
	const char* funcDef = "void printToCSV(const char*, char**, float**, float [], int, int)";
	ofstream csvOutputFile (CSV_FILENAME, ios::out);
    if(csvOutputFile.is_open()){
		// Headings
		csvOutputFile << "Wavenumber, ";
		for(int i = 0; i < NUM_SPA_FILES - 1; i++)
			csvOutputFile << SPA_FILENAME[i] << ", ";
		csvOutputFile << SPA_FILENAME[NUM_SPA_FILES - 1] << endl;

		// Data
		for(int i = 0; i < SIZE; i++)
		{
			csvOutputFile << wavenumber[i] << ", ";
			for(int j = 0; j < NUM_SPA_FILES - 1; j++)
				csvOutputFile << IR_Data[j][i] << ", ";
			csvOutputFile << IR_Data[NUM_SPA_FILES - 1][i] << endl;
		}
		csvOutputFile.close();
	} else {
		cerr << "Error: " << funcDef << ": unable to open output file '" << CSV_FILENAME << "'.\n"
			 << "    Does the file already exist?\n";
		exit(1);
	}
	return;
}

// One bound specified
void printToCSV
(
	const char* CSV_FILENAME,
	char** SPA_FILENAME, 
	float** IR_Data,
	float wavenumber[],
	int NUM_SPA_FILES,
	int SIZE,
	int bound,
	bool upperBoundSpecified
)
{
	const char* funcDef = "void printToCSV(const char*, char**, float**, float [], int, int, int, bool)";
	int boundIndex = wavenumToIndex(bound, wavenumber, SIZE);
	ofstream csvOutputFile (CSV_FILENAME, ios::out);
	if(csvOutputFile.is_open())
	{
		// Headings
		csvOutputFile << "Wavenumber, ";
		for(int i = 0; i < NUM_SPA_FILES - 1; i++)
			csvOutputFile << SPA_FILENAME[i] << ", ";
		csvOutputFile << SPA_FILENAME[NUM_SPA_FILES - 1] << endl;
		// Data
		if(upperBoundSpecified)
			for(int i = boundIndex; i < SIZE; i++)
			{
				csvOutputFile << wavenumber[i] << ", ";
				for(int j = 0; j < NUM_SPA_FILES - 1; j++)
					csvOutputFile << IR_Data[j][i] << ", ";
				csvOutputFile << IR_Data[NUM_SPA_FILES - 1][i] << endl;
			}
		else
			for(int i = 0; i < boundIndex + 1; i++)
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
		cerr << "Error: " << funcDef << ": unable to open output file '" << CSV_FILENAME << "'.\n"
			 << "    Does the file already exist?\n";
		exit(1);
	}
	return;
}

// Both bound specified: print region of interest
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
	const char* funcDef = "void printToCSV(const char*, char**, float**, float [], int, int, int, int)";
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
		cerr << "Error: " << funcDef << ": unable to open output file '" << CSV_FILENAME << "'.\n"
			 << "    Does the file already exist?\n";
		exit(1);
	}
	return;
}

const char* createCSVFilename(const char* filename, string ubStr, string lbStr)
{
	string str = filename;
	return str.append(".").append(ubStr).append("-").append(lbStr).append(".CSV").c_str();
}

