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

// NOTE: requires strToInt.cpp (uses truncateStrAt(string, char))

void checkIfAlreadyGiven(int argIndex, bool* optionalArgs[], bool* usedOptionalArgs)
{
    *usedOptionalArgs = true;
    if(*optionalArgs[argIndex] == false)
        *optionalArgs[argIndex] = true;
    else
    {
        switch(argIndex)
        {
            case UB_ARG_INDEX: 
                cerr << "Error: checkIfAlreadyGiven(int, bool* [], bool*): Upper bound specified more than once.\n";
                break;
            case LB_ARG_INDEX:
                cerr << "Error: checkIfAlreadyGiven(int, bool* [], bool*): Lower bound specified more than once.\n";
                break;
            case CONST_CORR_ARG_INDEX:
                cerr << "Error: checkIfAlreadyGiven(int, bool* [], bool*): Calculate constant correction flag used more than once.\n";
                break;
            case GROUP_FILES_ARG_INDEX:
                cerr << "Error: checkIfAlreadyGiven(int, bool* [], bool*): Group files flag used more than once.\n";
                break;
            default:
                cerr << "Error: checkIfAlreadyGiven(int, bool* [], bool*): invalid argument index.\n";
        }
        exit(1);
    }
    return;
}

bool usingOptionalArgs
(
    int argc,
    char* argv[],
    const int NUM_OPT_ARGS,
    bool* optionalArgs[],
    int optionalArgIndices[]
)
{
    bool usedOptionalArgs = false;
    for(int i = 0; i < argc; i++)
    {
        string fullArg = argv[i];
        string argName = truncateStrAt(fullArg, ARG_VAL_DIV_CHAR);
        if(argName == UB_ARG_SHORT_STR || argName == UB_ARG_STR)
        {
            checkIfAlreadyGiven(UB_ARG_INDEX, optionalArgs, &usedOptionalArgs);
            optionalArgIndices[UB_ARG_INDEX] = i;
        }
        else if(argName == LB_ARG_SHORT_STR || argName == LB_ARG_STR)
        {
            checkIfAlreadyGiven(LB_ARG_INDEX, optionalArgs, &usedOptionalArgs);
            optionalArgIndices[LB_ARG_INDEX] = i;
        }
        else if(argName == CONST_CORR_STR)
        {
            checkIfAlreadyGiven(CONST_CORR_ARG_INDEX, optionalArgs, &usedOptionalArgs);
            optionalArgIndices[CONST_CORR_ARG_INDEX] = i;
        }
        else if(argName == GROUP_FILES_STR)
        {
            checkIfAlreadyGiven(GROUP_FILES_ARG_INDEX, optionalArgs, &usedOptionalArgs);
            optionalArgIndices[GROUP_FILES_ARG_INDEX] = i;
        }
    }
    return usedOptionalArgs;
}

void checkArgIndex(const int NUM_OPT_ARGS, int optionalArgIndices[], int maxIndex, int argc, char* argv[])
{
    for(int i = 0; i < NUM_OPT_ARGS; i++)
        if(optionalArgIndices[i] > maxIndex)
        {
            string optArg;
            switch(i)
            {
                case UB_ARG_INDEX: optArg = UB_ARG_STR; break;
                case LB_ARG_INDEX: optArg = LB_ARG_STR; break;
                case CONST_CORR_ARG_INDEX: optArg = CONST_CORR_STR; break;
                case GROUP_FILES_ARG_INDEX: optArg = GROUP_FILES_STR; break;
            }
            cerr << "Error: checkArgIndex(int, int [], int): index of optional argument '" << optArg << "' is larger than expected.\n\n";
            printUsage(argv[0]);
            exit(1);
        }
}

int checkArgOrder(const int NUM_OPT_ARGS, bool* optionalArgs[], int optionalArgIndices[], int argc, char* argv[])
{
    int numOptArgsSpecified = 0;
    for(int i = 0; i < NUM_OPT_ARGS; i++)
        if(*optionalArgs[i] == true) numOptArgsSpecified++;
    checkArgIndex(NUM_OPT_ARGS, optionalArgIndices, numOptArgsSpecified, argc, argv);
    return numOptArgsSpecified;
}