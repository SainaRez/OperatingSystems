#Operating Systems
#Project 1 checkpoint
#Author: Saina Rezvani, Josh Desmond
#Date: 01/31/2020

This project includes the boring.c, custom.c and multi.c files.

# File/Phase Descriptions: 

Phase 1: 
The boring.c runs the following commands: whoami, last, ls -al /home
It then prints the stats of the commands

Phase 2:
The custom.c reads and parses a text file that includes a number of commands and executes them all in order.

Phase 3:
The multi.c takes in integers as line numbers from the command line and reads and parses a text file that includes commands. It then executes the commands at the given line numbers in the background and the rest as forground. It then prints the stats of the commands and the processes.

# Outputs:

The output of the files/phases are in boring_output.txt, custom_ouput.txt and multi_output.txt

These files include the command that was ran, the content of the text file (custom.txt, multi.txt) and the output that was printed in the command line.

# Additional Information on Phase 3:

We execute a background command and while we are reading the next command/line we check to see if a background command is done running with wait functions. If it's done we print it, if not we move to the next line/command. 

If it's a forground process, we run it normally and wait on it. After we run the forground command, we check again for the background command to see if it's done until there is not a valid input for fgets. 

# Testing:

We tested boring and custom by trying different commands in different orders. We tested multi by testing it with different commands and different line numbers for bakcgorund processes. We also tried passing in more than 2 numbers or less than 2.


# Running the files:

Running boring:

$ make
$ ./boring


Running custom:

$ make
$ ./custom


Running multi:
$ make
$ ./multi 1 3

# Makefile includes all three programs and running make one time is sufficient