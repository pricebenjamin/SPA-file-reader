#ifndef READ_WRITE_H
#define READ_WRITE_H

#include <string>
// TODO(ben): make capitalization consistent

// no bounds specified
void printToCSV(
    const char* CSV_FILENAME, // TODO(ben): use strings
    char** SPA_FILENAME,
    float** IR_Data,          // TODO(ben): use vectors?
    float wavenumber[],
    int NUM_SPA_FILES,
    int SIZE
);

// upper-bound specified
void printToCSV(
    const char* CSV_FILENAME,
    char** SPA_FILENAME,
    float** IR_Data,
    float wavenumber[],
    int NUM_SPA_FILES,
    int length,
    int upperBound,
    int lowerBound
);

// upper- and lower-bound specified
void printToCSV(
    const char* CSV_FILENAME,
    char** SPA_FILENAME,
    float** IR_Data,
    float wavenumber[],
    int NUM_SPA_FILES,
    int upperBound,
    int lowerBound
);

// TODO(ben): create struct / calss for passing information to functions
void readSPAFile(char* SPA_FILENAME, float IR_Data[], int length, int start);
const char* createCSVFilename(
    const char* filename,
    std::string upperBoundStr,
    std::string lowerBoundStr
);

#endif // READ_WRITE_H
