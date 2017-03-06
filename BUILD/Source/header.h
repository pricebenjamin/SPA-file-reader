#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

void printUsage(char* PROG_NAME);
bool usingOptionalArgs(int argc, char** argv, const int numOptionalArgs, bool* optionalArgs[], int optionalArgIndices[]);
string truncateStrAt(string str, char truncAt);
string getStrAfter(string str, char dividingChar);
void checkArgOrder(int NUM_OPT_ARGS, bool* optionalArgs[], int optionalArgIndices[], int argc, char* argv[]);

#endif