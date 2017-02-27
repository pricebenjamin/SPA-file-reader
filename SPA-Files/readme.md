## SPA File Information

Using the Sublime Text 3 plugin "HexViewer" in combination with an [online hex converter](http://www.scadacore.com/field-applications/programming-calculators/online-hex-converter/), it appears that transmission data is stored as 32-bit floating point numbers in Mid-Big-Endian format. 

Transmission data starts at address `0x049E` and ends at address `0x036929`.

Each individual transmission datum is 4 bytes, yielding a total of 55,587 data points.

My specific IR spectra contain data between 649.9812 and 3999.9907 cm<sup>-1</sup>.
