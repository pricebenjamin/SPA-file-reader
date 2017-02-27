# Things I've found out

Using the Sublime Text 3 plugin "HexViewer" in combination with an [online hex converter](http://www.scadacore.com/field-applications/programming-calculators/online-hex-converter/), it appears that % transmission data is stored as 32-bit floating point numbers in Mid-Big-Endian format (BADC). 

% Transmission data beings at address 0x049E (inclusive) and ends at 0x036929 (inclusive); each datum is 4 bytes. This gives a total of 55,587 measurements.

My specific IR spectra contain data between 649.9812 and 3999.9907 cm^-1.
