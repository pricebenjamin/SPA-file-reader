# SPA-file-reader

SPA is the default file type of Thermo Scientific(TM) OMNIC Series Software(TM). These files
store infrared (IR) spectral data in a binary format, which makes data analysis and processing
unnecessarily difficult without OMNIC Series Software(TM).

As a student without home-access to OMNIC software (and who may or may not have saved all of
his IR data in the SPA format), I wanted to find a way to extract my spectral data and save
them in an easy-to-process and human-readable format. Thankfully, a Cprogramming user, JonathanS,
[put together a functional C++ program][cprog] capable of extracting these data. SPA-file-reader
is a continuation of Jonathan's work.

Currently, this program is entirely capable of converting the SPA files found in the 'SPA-Files'
folder to CSV files. The spectra were taken using a Nicolet iS10 FT-IR spectrometer. It is
unlikely, however, that this program will work as-is for SPA files generated by a different
spectrometer; some effort would need to be put forth in order to determine the offset addresses
where transmission/absorption data begins and ends, the step-size (in inverse centimeters)
between consecutive data, the smallest or largest wavenumber at which transmission/absorption
was measured, and the order and format of the data.

[cprog]: https://cboard.cprogramming.com/cplusplus-programming/152474-reading-ir-spectrosopy-file-spa-file-unknown-binary-file.html


## How to use this program

Once compiled, you can view the usage statement from the command-line in Windows or Linux by
executing the program without any arguments, or with the optional arguments `-h`, `-?`, or `--help`.

### Compiling the latest source files (located in `src`)

This program has been successfully compiled on Linux (Ubuntu 18.04 LTS) using g++ and on Windows
using Visual Studio Build Tools 2017 RC. Once the repository has been cloned, users should change
directory into the `src` folder from the command-line. From there, they can execute the following
commands to compile the source files into an executable.

#### On Windows (Developer Command Prompt for VS 2017 RC)
```
> cl /EHsc main-with-new-cla.cpp data-processing.cpp parse-command-line-args.cpp print-usage.cpp read-write.cpp str-to-int.cpp /link /out:spa-reader.exe
```

#### On Linux
```
$ g++ -std=c++11 main-with-new-cla.cpp data-processing.cpp parse-command-line-args.cpp print-usage.cpp read-write.cpp str-to-int.cpp -o spa-reader
```

### Using the old source files (located in `src/old`)

Assuming a user has access to the g++ compiler, they may compile and run this program by
following the provided steps.

1. Download the get-ir-mwv.cpp source file.
2. From a terminal, change directory to the folder which contains the downloaded source file.
3. Compile the source file into an executable with the following command:`g++ get-ir-mwv.cpp -o get-ir-mwv`
4. Move the SPA file from which data should be extracted into the directory containing the executable.
5. Run the executable on the SPA file with the following command: `./get-ir-mwv <SPA Filename>`

Example:
```
./get-ir-mwv 0min-1-97C.SPA
```

If everything works, the user should now have a new file in the current directory named
`<SPA Filename>.CSV` (e.g. `0min-1-97C.SPA.CSV`) which contains the extracted transmission or
absorption data.

If a user has multiple SPA files in the same directory, they may move the executable into that
directory and process all SPA files at once by entering the following into a terminal running bash:
```
for FILE in *.SPA
  do
    ./get-ir-mwv $FILE
  done
```
