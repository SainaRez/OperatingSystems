#Operating Systems
#Project 2: Memory Manager
#Author: Saina Rezvani, Josh Desmond
#Date: 02/14/2020

This project includes the main.c files.

# File/Phase Descriptions: 

Part 1: 
The program prints the instruction prompt and takes in a command from the user in command line. If it's a map command it creates a page table at the given address and maps a page in the virtual memory to the physical memory. If it's store, it takes the value and if the memory at the given virtual address is mapped, it stores that value at the associated physical address. If it's load, it loads the value that is stored in the given virtual address. 

Part 2:
When the physical memory is full and a map command is given, the program swaps a page that is not holding a page table to the disk. If all the pages are holding a page a table, then it swaps the page table to the disk. It then maps the newly empty memory slot with the given address. When the commands: map, store and load are called where the given page exists on disk, the program swaps that page back into the memory. If the page table is on the disk as well, it swaps the page table first and then it swaps the page.


# Outputs:

The outputs of running our test file "Test.sh" are recorded in the output file, test_output.txt.


# Testing:

Test scripts are included as test_commands.txt, test_simple.txt, and test_extended.txt. Direction to run the test files is in next section.


# Running the files:

Running Memory Manager:

$ make
$ ./main


Running Tests: (They can be run individually)

$ make
$ ./main < ./test_commands.txt
$ ./main < ./test_simple.txt
$ ./main < ./test_extended.txt

To run all three test scripts together, run the bash script Test.sh


# Makefile includes multiple files, but main.c is the final and correct version of the Memory Manager program.