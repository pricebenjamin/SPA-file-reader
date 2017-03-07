#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

void printUsage(char* PROG_NAME);

// parse-command-line-args.cpp
bool usingOptionalArgs(int argc, char** argv, const int numOptionalArgs, bool* optionalArgs[], int optionalArgIndices[]);
int checkArgOrder(int NUM_OPT_ARGS, bool* optionalArgs[], int optionalArgIndices[], int argc, char* argv[]);
// str-to-int.cpp
string truncateStrAt(string str, char truncAt);
string getStrAfter(string str, char dividingChar);
int strToInt(string numberAsString);
// read-write.cpp
void printToCSV
(
	const char* CSV_FILENAME,
	char** SPA_FILENAME,
	float** IR_Data,
	float wavenumber[],
	int NUM_SPA_FILES,
	int SIZE
); // Used when no bounds specified
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
); // Used when both bounds specified
void readSPAFile(char* SPA_FILENAME, float IR_Data[], int length, int start);
const char* createCSVFilename(const char* filename, string ubStr, string lbStr);
// create-array.cpp
char** createSPAFileArray(int NUM_SPA_FILES, int numOptArgs, char* argv[], const char* ptrDef);
float** createFloatArray(int numCols, int numRows, const char* ptrDef);
char** createAvgDataColTitles(int numGroups, int groupSize, char** SPA_FILENAME, const char* ptrDef);
// data-processing.cpp
void checkBound(int* upperBound, int* lowerBound, int max, int min);
void checkBound(int bound, int MAX_WAVENUMBER, int MIN_WAVENUMBER);
int wavenumToIndex(int wavenumber, float wavenumberArray[], int size);
void computeAverages(float** AVG_DATA, float** IR_DATA, int numGroups, int groupSize, int SIZE);
void computeConstCorr(float** CORR_DATA, float** IR_DATA, int NUM_SPA_FILES, float WAVENUMBER[], int SIZE, int ubCorr, int lbCorr);

#endif