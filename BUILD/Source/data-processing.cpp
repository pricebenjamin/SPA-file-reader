#include <iostream>
#include <cstdlib>
#include <string> // createFloatArray(): to_string(), string()
#include <cmath> // wavenumToIndex(): abs()

using namespace std;

// Ensure that ub > lb, ub <= MAX_WAVENUMBER, lb >= MIN_WAVENUMBER, etc...
void checkBound(int* upperBound, int* lowerBound, float max, float min)
{
	const char* funcDef = "checkBound(int*, int*, int, int)";
	if(*upperBound == *lowerBound) // Dereference and compare
	{
		cerr << "Error: " << funcDef << ": upperbound equals lowerbound." << endl;
		exit(1);
	} else if(*upperBound < *lowerBound) {// Swap value stored in upperbound with value in lowerbound
		int temp = *upperBound;
		*upperBound = *lowerBound;
		*lowerBound = temp;
	}
	// Once swapped, check for invalid bounds
	if(*upperBound > max && *lowerBound < min) {
		cerr << "Error: " << funcDef << ": upperbound > MAX_WAVENUMBER and lowerbound < MIN_WAVENUMBER." << endl;
		exit(1);
	} else if(*upperBound > max) {
		cerr << "Error: " << funcDef << ": upperbound > MAX_WAVENUMBER." << endl;
		exit(1);
	} else if(*lowerBound < min) {
		cerr << "Error: " << funcDef << ": lowerbound < MIN_WAVENUMBER." << endl;
		exit(1);
	}
}

void checkBound(int bound, float MAX_WAVENUMBER, float MIN_WAVENUMBER)
{
	const char* funcDef = "void checkBound(int, int, int)";
    if(bound >= MAX_WAVENUMBER)
    {
        cerr << "Error: " << funcDef << ": bound = " << bound << " is greater than or equal to max wavenumber " << MAX_WAVENUMBER << ".\n";
        exit(1);
    }
    else if(bound <= MIN_WAVENUMBER)
    {
        cerr << "Error: " << funcDef << ": bound = " << bound << " is less than or equal to min wavenumber " << MIN_WAVENUMBER << ".\n";
        exit(1);
    }
    return;
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

void checkIfNull(void* pointer, const char* callingFunc, const char* ptrDef)
{
	if(pointer == nullptr)
	{
		cerr << "Error: " << callingFunc << ": unable to allocate memory for " << ptrDef << ".\n";
		exit(1);
	}
	return;
}

char** createSPAFileArray(int NUM_SPA_FILES, int numOptArgs, char* argv[], const char* ptrDef)
{
    const char* funcDef = "char** createSPAFileArray(int, int, char* [], const char*)";
    char** filenameArray;
    filenameArray = new (nothrow) char* [NUM_SPA_FILES];
    checkIfNull(filenameArray, funcDef, ptrDef);
    for(int i = 0; i < NUM_SPA_FILES; i++)
        filenameArray[i] = argv[i + numOptArgs + 1];
    return filenameArray;
}

char** createAvgDataColTitles(int numGroups, int groupSize, char** SPA_FILENAME, const char* ptrDef)
{
    const char* funcDef = "char** createAvgDataColTitles(int, int, char**, const char*)";
    char** colTitles = new (nothrow) char* [numGroups];
    checkIfNull(colTitles, funcDef, ptrDef);
    for(int i = 0; i < numGroups; i++)
        colTitles[i] = SPA_FILENAME[i*groupSize];
    return colTitles;
}

float** createFloatArray(int numCols, int numRows, const char* ptrDef)
{
    const char* funcDef = "float** createFloatArray(int, int, const char*)";
    float** floatArray;
    floatArray = new (nothrow) float* [numCols];
    checkIfNull(floatArray, funcDef, ptrDef);
    string ptrDefStr = ptrDef;
    for(int i = 0; i < numCols; i++)
    {
        floatArray[i] = new (nothrow) float [numRows];
        ptrDefStr += string(": floatArray[") + to_string(i) + string("] = new float [") + to_string(numRows) + string("]");
        const char* tempPtrDef = ptrDefStr.c_str();
        checkIfNull(floatArray[i], funcDef, tempPtrDef);
    }
    return floatArray;
}

void computeAverages(float** AVG_DATA, float** IR_DATA, int numGroups, int groupSize, int SIZE)
{
    for(int i = 0; i < SIZE; i++)
        for(int j = 0; j < numGroups; j++)
        {
            float sum = 0;
            for(int k = 0; k < groupSize; k++)
                sum += IR_DATA[j*groupSize + k][i];
            AVG_DATA[j][i] = sum / (float)groupSize;
        }
    return;
}

void computeConstCorr(float** CORR_DATA, float** IR_DATA, int NUM_SPA_FILES, float WAVENUMBER[], int SIZE, int ubCorr, int lbCorr)
{
    const char* funcDef = "void computeConstCorr(float**, float**, int, int, int, int)";
    float* baseline = new (nothrow) float [SIZE];
    checkIfNull(baseline, funcDef, "float* baseline");
    for(int i = 0; i < SIZE; i++)
    {
        float sum = 0;
        for(int j = 0; j < NUM_SPA_FILES; j++)
            sum += IR_DATA[j][i];
        baseline[i] = sum / (float)NUM_SPA_FILES;
    }

    float** difference = createFloatArray(NUM_SPA_FILES, SIZE, (string(funcDef) + string(": ") + string("float** difference")).c_str());
    for(int i = 0; i < NUM_SPA_FILES; i++)
        for(int j = 0; j < SIZE; j++)
            difference[i][j] = baseline[j] - IR_DATA[i][j];
    
    int lbCorrIndex = wavenumToIndex(ubCorr, WAVENUMBER, SIZE);
    int ubCorrIndex = wavenumToIndex(lbCorr, WAVENUMBER, SIZE);

    float* averageDiffOverInterval = new (nothrow) float [NUM_SPA_FILES];
    checkIfNull(averageDiffOverInterval, funcDef, "float* averageDiffOverInterval");
    for(int i = 0; i < NUM_SPA_FILES; i++)
    {
        float sum = 0;
        for(int j = lbCorrIndex; j < ubCorrIndex + 1; j++)
            sum += difference[i][j];
        averageDiffOverInterval[i] = sum / (float)(ubCorrIndex - lbCorrIndex + 1);
    }

    for(int i = 0; i < NUM_SPA_FILES; i++)
        for(int j = 0; j < SIZE; j++)
            CORR_DATA[i][j] = averageDiffOverInterval[i] + IR_DATA[i][j];
    delete[] baseline;
    return;
}
