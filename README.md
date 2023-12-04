# compilation.c

Program which takes as input WAV files and outputs a compilation with transitions and background sounds.

# Building

Type `make` in the downloaded folder.

# Usage

`./compilation file1.wav file2.wav ... fileN.wav -b [backgroundFile] -v [backgroundVolume] -o [outputName]`

- -b is used to select the background sounds/music used in the final compilation. 

- -v is an optional argument used to control the volume of the background sounds. Set this to .5 for 50%, 2 for 200%, etc. The default value is 1.

- -o is an optional argument used to choose the outputted file's name and its folder location. The default is to save the file in the current directory with the name "output.wav".

This program only works properly for WAV files which are stereo, of the same sample rate, and have 2-byte samples.
