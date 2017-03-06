#include "header.h"

const string UB_ARG_STR = "--upper-bound";
const string LB_ARG_STR = "--lower-bound";
const string UB_ARG_SHORT_STR = "-u";
const string LB_ARG_SHORT_STR = "-l";
const string CONST_CORR_STR = "--calculate-const-corr";
const string GROUP_FILES_STR = "--group-files";

const int UB_ARG_INDEX = 0;
const int LB_ARG_INDEX = 1;
const int CONST_CORR_ARG_INDEX = 2;
const int GROUP_FILES_ARG_INDEX = 3;

const char ARG_VAL_DIV_CHAR = '=';
const char VAL_VAL_DIV_CHAR = '-';

void printUsage(char* PROG_NAME)
{
	cerr << "USAGE: " << PROG_NAME << "  [ OPTIONS ... ] file1 [ file2 ... ]\n"
         << "DESCRIPTION:\n"
         << "    Reads % transmission or % absorption values from SPA files created by Thermo\n"
         << "    Scientific OMNIC software and writes them to a CSV file. Options allow the user to\n"
         << "    save only a specified region of each spectrum in the CSV file, calculate and apply a\n"
         << "    constant correction to each spectrum (saved in separate file), and take the average\n"
         << "    of multiple spectra by grouping files (saved in separate file).\n"
         << "OPTIONS:\n"
	     << "    -u=N1, --upper-bound=N1        Defines the upper bound N1 of the wavenumber region\n"
         << "                                     over which data will be saved.\n"
	     << "    -l=N2, --lower-bound=N2        Defines the lower bound N2 of the wavenumber region\n"
         << "                                     over which data will be saved.\n"
         << "    --calculate-const-corr=N3-N4   Define a wavenumber region between N3 and N4 which\n"
         << "                                     will be used to calculate a constant correction.\n"
         << "                                     The corrections will try to move each spectrum\n"
         << "                                     up or down in order to minimize the distance\n"
         << "                                     between spectra in this region.\n"
         << "    --group-files=N5               Define the number of files N5 which will be grouped\n"
         << "                                     and averaged. (Expects that user passes a multiple\n"
         << "                                     of N5 total files.)\n";
	return;
}

int main(int argc, char* argv[])
{
    // Expected usage of this program:
    // ./PROG_NAME [-u=<upper bound>] [-l=<lower bound>] [--calculate-const-corr=<bound>-<bound>] [--group-files=<group size>] <SPA filename 1> <SPA filename 2> ...

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
    
    checkArgOrder(NUM_OPT_ARGS, optionalArgs, optionalArgIndices, argc, argv);

    if(optionalArgsUsed)
    {
        if(upperBoundSpecified && lowerBoundSpecified)
        {
            for(int i = 0; i < NUM_OPT_ARGS; i++)
                if(*optionalArgs[i] == true)
                    cout << "Found: " << argv[optionalArgIndices[i]] << ".\n";
            // Create raw data CSV

            // Create corrected data CSV if specified

            // Create averaged data CSV if specified

            // Create corrected averaged data if specified
        }
        else if (upperBoundSpecified || lowerBoundSpecified)
        {
            cerr << "Error: main(): scenario: one bound specified.\n";
            cerr << "    This scenario is still under development.\n";
        }
        else
        {
            cerr << "Error: main(): scenario: no bounds specified.\n";
            cerr << "    This scenario is still under development.\n";
        }
    }
    else
    {
        cerr << "Error: main(): scenario: no optional arguments used.\n";
        cerr << "   This scenario is still under development.\n";
    }

    return 0;
}
