#ifndef PARSE_CMD_H
#define PARSE_CMD_H

// TODO(ben): package optinal args in struct / class
bool usingOptionalArgs(
    int argc,
    char* argv[],
    const int numOptionalArgs,
    bool* optionalArgs[],
    int optionalArgIndices[]
);

int checkArgOrder(
    int NUM_OPT_ARGS,
    bool* optionalArgs[],
    int optionalArgIndices[],
    int argc,
    char* argv[]
);

#endif // PARSE_CMD_H
