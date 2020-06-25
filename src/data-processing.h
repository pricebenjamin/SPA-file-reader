#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

void checkBound(int* upperBound, int* lowerBound, float MAX_WAVENUMBER, float MIN_WAVENUMBER);
void checkBound(int bound, float MAX_WAVENUMBER, float MIN_WAVENUMBER);
int wavenumToIndex(int wavenumber, float wavenumberArray[], int size);
void computeAverages(float** AVG_DATA, float** IR_DATA, int numGroups, int groupSize, int SIZE);
void computeConstCorr(
    float** CORR_DATA,
    float** IR_DATA,
    int NUM_SPA_FILES,
    float WAVENUMBER[],
    int SIZE,
    int upperBoundCorrection,
    int lowerBoundCorrection
);

char** createSPAFileArray(int NUM_SPA_FILES, int numOptArgs, char* argv[], const char* ptrDef);
float** createFloatArray(int numCols, int numRows, const char* ptrDef);
char** createAvgDataColTitles(int numGroups, int groupSize, char** SPA_FILENAME, const char* ptrDef);

#endif // DATA_PROCESSING_H
