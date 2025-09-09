To compile this program on a linux command line, cd into a directory with all of the source code files (main.cpp, Cell.h, Cell.cpp) and Makefile, and run the command "make"
This will produce an executable called "main"
On GNU/Linux, simply run the command "./main" to run the program

Please make sure that the region file and the config file are in the same directory as the executable

The program will prompt for the name of the config file, and will do the rest automatically, producing output for each round and the pollution spread
The program will then ask you if you would like to analyze a specific portion of the region
The program will continue prompting to analyze a region until the character "n" is inputted when prompted, at which point the program will terminate