# Things I've found out
## SPA File Information

Using the Sublime Text 3 plugin "HexViewer" in combination with an [online hex converter](http://www.scadacore.com/field-applications/programming-calculators/online-hex-converter/), it appears that transmission data is stored as 32-bit floating point numbers in Mid-Big-Endian format. 

% Transmission data beings at address `0x049C` (inclusive) and ends at `0x036927` (inclusive); each datum is 4 bytes.
This gives a total of 55,587 measurements: `(end_addr - start_addr + 1) / 4 = 55,587`.

My specific IR spectra contain data between 649.9812 and 3999.9907 cm<sup>-1</sup>.

Binary playground:

0000 0000 0000 0000 0000 0000 0000 0000
23d9 0000 0000 0000 daff 7945 cc7e 2244
0000 0000 0000 0000 0000 0000 0000 0000
0000 0000 0000 0000 0000 0000 933a c842
0e3d c842 ba3d c842 b83c c842 c13a c842
f738 c842 8338 c842 243a c842 da3d c842
e042 c842 f647 c842 c64b c842 454d c842
2a4c c842 b048 c842 9743 c842 b33d c842
cc37 c842 7032 c842 fc2d c842 bd2a c842
e928 c842 ae28 c842 f629 c842 612c c842
442f c842 c831 c842 3833 c842 2e33 c842
c331 c842 742f c842 f72c c842 e82a c842
398b c242 7619 c342 219f c342 8a10 c442