# SPA-file-reader

SPA is the default file type of Thermo Scientific(TM) OMNIC Series Software(TM). These files store infrared (IR) spectral data in a binary format, which makes data analysis and processing nearly impossible without OMNIC Series Software(TM).

As a student without home access to OMNIC software (and who saved all of his IR spectral data in the SPA format), I need a way to extract my spectral data and save them in a human-readable format. Thankfully, a Cprogramming user, JonathanS, put together a functional C++ program capable of extracting these data (link: https://cboard.cprogramming.com/cplusplus-programming/152474-reading-ir-spectrosopy-file-spa-file-unknown-binary-file.html); SPA-file-reader is a continuation of Jonathan's work.

Currently, this program is entirely capable of converting the SPA files found in the 'SPA-Files' folder to CSV files. It is unlikely, however, that this program will work as-is for SPA files generated by a different spectrometer; a small amount of effort would need to be put forth in order to determine the offset addresses where transmission/absorption data begins and ends.

## Using this program

Assuming a user has access to the g++ compiler, they may compile and run this program by following the provided steps.

1. Download the get-ir-mwv.cpp source file.
2. From a terminal, change directory to the folder which contains the downloaded source file.
3. Compile the source file into an executable with the following command:`g++ get-ir-mwv.cpp -o get-ir-mwv`
4. Move the SPA file from which data should be extracted into the directory containing the executable.
5. Run the executable on the SPA file with the following command: `./get-ir-mwv <SPA Filename>`

Example:
```
./get-ir-mwv 0min-1-97C.SPA
```

If everything works, the user should now have a new file in the current directory named `<SPA Filename>.CSV` (e.g. `0min-1-97C.SPA.CSV`) which contains the extracted transmission or absorption data.

If a user has multiple SPA files in the same directory, they may move the executable into that directory and process all SPA files at once by entering the following into a terminal the bash shell:
```
for FILE in *.SPA
  do
    ./get-ir-mwv $FILE
  done
```
