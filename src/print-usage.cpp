#include <iostream>

void printUsage(char* PROG_NAME)
{
    std::cerr << "USAGE: " << PROG_NAME << "  [options...] file...\n\n"
         << "DESCRIPTION:\n\n"
         << "    Reads % transmission or % absorption values from SPA files created by Thermo\n"
         << "    Scientific OMNIC software and writes them to a CSV file. Options allow the user to\n"
         << "    save only a specified region of each spectrum in the CSV file, calculate and apply a\n"
         << "    constant correction to each spectrum (saved in separate file), and take the average\n"
         << "    of multiple spectra by grouping files (saved in separate file).\n\n"
         << "OPTIONS:\n\n"
         << "    -h, -?, --help                 Print this usage statement.\n\n"
	     << "    -u=N1, --upper-bound=N1        Defines the upper bound N1 of the wavenumber region\n"
         << "                                   over which data will be saved.\n\n"
	     << "    -l=N2, --lower-bound=N2        Defines the lower bound N2 of the wavenumber region\n"
         << "                                   over which data will be saved.\n\n"
         << "    --calculate-const-corr=N3-N4   Define a wavenumber region between N3 and N4 which\n"
         << "                                   will be used to calculate a constant correction.\n"
         << "                                   The corrections will try to move each spectrum\n"
         << "                                   up or down in order to minimize the distance\n"
         << "                                   between spectra in this region.\n\n"
         << "    --group-files=N5               Define the number of files N5 which will be grouped\n"
         << "                                   and averaged. (Expects that user passes a multiple\n"
         << "                                   of N5 total files.)\n\n";
}
