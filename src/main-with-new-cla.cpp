#include "data-processing.h"
#include "parse-command-line-args.h"
#include "print-usage.h"
#include "read-write.h"
#include "str-to-int.h"

#include <iostream>

// TODO(ben): stdlib imports

const std::string UB_ARG_STR = "--upper-bound";
const std::string LB_ARG_STR = "--lower-bound";
const std::string UB_ARG_SHORT_STR = "-u";
const std::string LB_ARG_SHORT_STR = "-l";
const std::string CONST_CORR_STR = "--calculate-const-corr";
const std::string GROUP_FILES_STR = "--group-files";

const int UB_ARG_INDEX = 0;
const int LB_ARG_INDEX = 1;
const int CONST_CORR_ARG_INDEX = 2;
const int GROUP_FILES_ARG_INDEX = 3;

const char ARG_VAL_DIV_CHAR = '=';
const char VAL_VAL_DIV_CHAR = '-';

const int HEX_START = 0x49C;		// Hex address of first byte of first datum in SPA file
const int HEX_END = 0x036927;		// Hex address of last byte of last datum
const int SIZE = (HEX_END - HEX_START + 1) / 4;	// Number of data in SPA file (+ 1 includes last byte, / 4 bytes per float)
const float MAX_WAVENUMBER =  3999.9907;	// inverse cm
const float MIN_WAVENUMBER = 649.9812;		// inverse cm
const float STEP_SIZE = (MAX_WAVENUMBER - MIN_WAVENUMBER) / (SIZE - 1);	// inverse cm; distance between sequential data 
                                                                        // (- 1 ensures that last datum gets last wavenum)

int main(int argc, char* argv[])
{
    // Expected usage of this program:
    // ./PROG_NAME [-u=<upper bound>] [-l=<lower bound>] [--calculate-const-corr=<bound>-<bound>] [--group-files=<group size>] <SPA filename 1> <SPA filename 2> ...

    // Check for 'help' flags
    if(argc < 2)
    {
    	printUsage(argv[0]);
    	return 0;
    }
    else
    {
        std::string argv_1 = argv[1];
	    if(argv_1 == "-h" || argv_1 == "-?" || argv_1 == "--help")
	    {
	    	printUsage(argv[0]);
	    	return 0;
	    }
	}

    const int NUM_OPT_ARGS = 4;
    const int MAX_OPT_ARG_INDEX = 4;

    bool upperBoundSpecified = false;
    bool lowerBoundSpecified = false;
    bool useConstCorr = false;
    bool groupFiles = false;

    bool* optionalArgs[] = {
        &upperBoundSpecified,
        &lowerBoundSpecified,
        &useConstCorr,
        &groupFiles
    }; // NOTE: ordering of these pointers affects *_ARG_INDEX values in this file and parse-command-line-args.cpp

    int optionalArgIndices[] = {0,0,0,0};

    bool optionalArgsUsed = 
        usingOptionalArgs(argc, argv, NUM_OPT_ARGS, optionalArgs, optionalArgIndices);
    
    // Check that SPA filenames are not interspersed between optional arguments 
    // and get the number of optional args specified
    int numOptArgsGiven = checkArgOrder(NUM_OPT_ARGS, optionalArgs, optionalArgIndices, argc, argv);
    const int NUM_SPA_FILES = argc - (numOptArgsGiven + 1);

    // Get data from SPA files
    // If no acceptable optional arguments were used, we will assume that all arguments are SPA files, and begin reading them in
    char** SPA_FILENAME = createSPAFileArray(NUM_SPA_FILES, numOptArgsGiven, argv, "char** SPA_FILENAME");
    float** IR_DATA = createFloatArray(NUM_SPA_FILES, SIZE, "float** IR_DATA");

    for(int i = 0; i < NUM_SPA_FILES; i++)
        readSPAFile(SPA_FILENAME[i], IR_DATA[i], SIZE, HEX_START);

	float WAVENUMBER[SIZE]; // Array to store corresponding wavenumber (assumed to be the same for all input files)
    
	for(int i = 0; i < SIZE; i++)
		WAVENUMBER[i] = MAX_WAVENUMBER - (STEP_SIZE * i);

    int groupSize = (groupFiles ? 
        (strToInt(getStrAfter(std::string(argv[optionalArgIndices[GROUP_FILES_ARG_INDEX]]), ARG_VAL_DIV_CHAR))) : 1);
    if(groupFiles && NUM_SPA_FILES % groupSize != 0)
    {
        std::cerr << "Error: main(): group files flag given, but given number of SPA files cannot be divided by group size.\n";
        exit(1);
    }

    int numGroups = NUM_SPA_FILES / groupSize;

    char** AVG_DATA_COL_TITLES = ( groupFiles ?
        createAvgDataColTitles(numGroups, groupSize, SPA_FILENAME, "char** AVG_DATA_COL_TITLES") : nullptr );
    float** AVG_DATA = ( groupFiles ?
        createFloatArray(numGroups, SIZE, "float** AVG_DATA") : nullptr );
    float** CORR_DATA = ( useConstCorr ?
        createFloatArray(NUM_SPA_FILES, SIZE, "float** CORR_DATA") : nullptr );
    
    std::string ubStr = ( upperBoundSpecified ?
        getStrAfter(std::string(argv[optionalArgIndices[UB_ARG_INDEX]]), ARG_VAL_DIV_CHAR) : "NULL_STRING" );
    std::string lbStr = ( lowerBoundSpecified ?
        getStrAfter(std::string(argv[optionalArgIndices[LB_ARG_INDEX]]), ARG_VAL_DIV_CHAR) : "NULL_STRING" );
    int upperBound = ( upperBoundSpecified ?
        strToInt(ubStr) : 0 );
    int lowerBound = ( lowerBoundSpecified ?
        strToInt(lbStr) : 0 );
    
    if(upperBoundSpecified && lowerBoundSpecified) 
        checkBound(&upperBound, &lowerBound, MAX_WAVENUMBER, MIN_WAVENUMBER);
    else if(upperBoundSpecified || lowerBoundSpecified) 
        upperBoundSpecified ? checkBound(upperBound, MAX_WAVENUMBER, MIN_WAVENUMBER) : checkBound(lowerBound, MAX_WAVENUMBER, MIN_WAVENUMBER);
    
    int ubCorr = ( useConstCorr ?
        strToInt(truncateStrAt(getStrAfter(std::string(argv[optionalArgIndices[CONST_CORR_ARG_INDEX]]), ARG_VAL_DIV_CHAR), VAL_VAL_DIV_CHAR)) : 0 );
    int lbCorr = ( useConstCorr ?
        strToInt(getStrAfter(getStrAfter(std::string(argv[optionalArgIndices[CONST_CORR_ARG_INDEX]]), ARG_VAL_DIV_CHAR), VAL_VAL_DIV_CHAR)) : 0 );
    if(useConstCorr) checkBound(&ubCorr, &lbCorr, MAX_WAVENUMBER, MIN_WAVENUMBER);

    // Output requested data
    if(optionalArgsUsed)
    {
    	//cout << "Outputting requested data: optionalArgsUsed has value of " << (optionalArgsUsed ? "True" : "False") << endl;
        if(upperBoundSpecified && lowerBoundSpecified)
        { // SCENARIO: both bounds given
            // Create raw data CSV
            const char* RAW_CSV_FILENAME = createCSVFilename("combinedRawData", ubStr, lbStr);
            printToCSV(RAW_CSV_FILENAME, SPA_FILENAME, IR_DATA, WAVENUMBER, NUM_SPA_FILES, SIZE, upperBound, lowerBound);
            // Create averaged data CSV if specified
            if(groupFiles)
            {
                computeAverages(AVG_DATA, IR_DATA, numGroups, groupSize, SIZE);
                const char* AVG_CSV_FILENAME = createCSVFilename("averagedData", ubStr, lbStr);
                printToCSV(AVG_CSV_FILENAME, AVG_DATA_COL_TITLES, AVG_DATA, WAVENUMBER, numGroups, SIZE, upperBound, lowerBound);
            }
            // Create corrected data CSV if specified
            if(useConstCorr)
            {
                computeConstCorr(CORR_DATA, IR_DATA, NUM_SPA_FILES, WAVENUMBER, SIZE, ubCorr, lbCorr);
                const char* CORR_CSV_FILENAME = createCSVFilename("constCorrData", ubStr, lbStr);
                printToCSV(CORR_CSV_FILENAME, SPA_FILENAME, CORR_DATA, WAVENUMBER, NUM_SPA_FILES, SIZE, upperBound, lowerBound);
            }
            // Create corrected averaged data if specified
            if(useConstCorr && groupFiles)
            {
                computeAverages(AVG_DATA, CORR_DATA, numGroups, groupSize, SIZE);
                const char* AVG_CORR_CSV_FILENAME = createCSVFilename("averagedCorrData", ubStr, lbStr);
                printToCSV(AVG_CORR_CSV_FILENAME, AVG_DATA_COL_TITLES, AVG_DATA, WAVENUMBER, numGroups, SIZE, upperBound, lowerBound);
            }
        }
        else if (upperBoundSpecified || lowerBoundSpecified)
        { // SCENARIO: one bound given
            std::string boundStr = ( upperBoundSpecified ? (std::string(".upperBound.") + ubStr) : (std::string(".lowerBound.") + lbStr) );
            int bound = ( upperBoundSpecified ? upperBound : lowerBound );
            const char* RAW_CSV_FILENAME = (std::string("combinedRawData") + boundStr + std::string(".CSV")).c_str();
            printToCSV(RAW_CSV_FILENAME, SPA_FILENAME, IR_DATA, WAVENUMBER, NUM_SPA_FILES, SIZE, bound, upperBoundSpecified);

            if(groupFiles)
            {
                computeAverages(AVG_DATA, IR_DATA, numGroups, groupSize, SIZE);
                const char* AVG_CSV_FILENAME = (std::string("averagedData") + boundStr + std::string(".CSV")).c_str();
                printToCSV(AVG_CSV_FILENAME, AVG_DATA_COL_TITLES, AVG_DATA, WAVENUMBER, numGroups, SIZE, bound, upperBoundSpecified);
            }

            if(useConstCorr)
            {
                computeConstCorr(CORR_DATA, IR_DATA, NUM_SPA_FILES, WAVENUMBER, SIZE, ubCorr, lbCorr);
                const char* CORR_CSV_FILENAME = (std::string("constCorrData") + boundStr + std::string(".CSV")).c_str();
                printToCSV(CORR_CSV_FILENAME, SPA_FILENAME, CORR_DATA, WAVENUMBER, NUM_SPA_FILES, SIZE, bound, upperBoundSpecified);
            }

            if(useConstCorr && groupFiles)
            {
                computeAverages(AVG_DATA, CORR_DATA, numGroups, groupSize, SIZE);
                const char* AVG_CORR_CSV_FILENAME = (std::string("averagedCorrData") + boundStr + std::string(".CSV")).c_str();
                printToCSV(AVG_CORR_CSV_FILENAME, AVG_DATA_COL_TITLES, AVG_DATA, WAVENUMBER, numGroups, SIZE, bound, upperBoundSpecified);
            }
        }
        else
        { // SCENARIO: no bounds given
            const char* RAW_CSV_FILENAME = "combinedRawData.fullSpectrum.CSV";
            printToCSV(RAW_CSV_FILENAME, SPA_FILENAME, IR_DATA, WAVENUMBER, NUM_SPA_FILES, SIZE);

            if(groupFiles)
            {
                computeAverages(AVG_DATA, IR_DATA, numGroups, groupSize, SIZE);
                const char* AVG_CSV_FILENAME = "averagedData.fullSpectrum.CSV";
                printToCSV(AVG_CSV_FILENAME, AVG_DATA_COL_TITLES, AVG_DATA, WAVENUMBER, numGroups, SIZE);
            }

            if(useConstCorr)
            {
                computeConstCorr(CORR_DATA, IR_DATA, NUM_SPA_FILES, WAVENUMBER, SIZE, ubCorr, lbCorr);
                const char* CORR_CSV_FILENAME = "constCorrData.fullSpectrum.CSV";
                printToCSV(CORR_CSV_FILENAME, SPA_FILENAME, CORR_DATA, WAVENUMBER, NUM_SPA_FILES, SIZE);
            }
            
            if(useConstCorr && groupFiles)
            {
                computeAverages(AVG_DATA, CORR_DATA, numGroups, groupSize, SIZE);
                const char* AVG_CORR_CSV_FILENAME = "averagedCorrData.fullSpectrum.CSV";
                printToCSV(AVG_CORR_CSV_FILENAME, AVG_DATA_COL_TITLES, AVG_DATA, WAVENUMBER, numGroups, SIZE);
            }
        }
    }
    else
    { // SCENARIO: No optional arguments given
        const char* RAW_CSV_FILENAME = "combinedRawData.fullSpectrum.CSV";
        printToCSV(RAW_CSV_FILENAME, SPA_FILENAME, IR_DATA, WAVENUMBER, NUM_SPA_FILES, SIZE);
    }

    delete[] SPA_FILENAME;
    for(int i = 0; i < NUM_SPA_FILES; i++)
    {
        delete[] IR_DATA[i];
        if(useConstCorr) delete[] CORR_DATA[i];
    }

    delete[] IR_DATA;
    if(useConstCorr) delete[] CORR_DATA;
    if(groupFiles)
    {
        delete[] AVG_DATA_COL_TITLES;
        for(int i = 0; i < numGroups; i++)
            delete[] AVG_DATA[i];
        delete[] AVG_DATA;
    }
    return 0;
}
