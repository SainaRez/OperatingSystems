#include <stdio.h>

#define SIZE 128 // TODO what's SIZE
#define PROMPT "Instruction? "
#define MAX_PROCESSES 4

unsigned char memory[SIZE];

/** 
 * Each process will have a simulated hardware register pointing to the start of
 * their respective page tables. You can simulate these registers with an array
 * indexed by process id. Values are initialized to -1 when there is no page table
 * yet.
 **/
int page_table_register_array[MAX_PROCESSES];
// Initialize all page_table_registers to -1
for (each i from 0 to 3) {
	page_table_register_array[i] = -1;
}

/**
 * Returns true if the given process has a page table initialized into memory yet
 */
bool does_process_have_page_file(int process_id) {
	return (page_table_register_array[process_id] == -1);
}

/**
 * TODO
 */
int load(int process_id, int virtual_address) {

}

/**
 * TODO
 */
void store(int process_id, int virtual_address, int value) {

}


/**
 * TODO
 *
 * @param int process_id
 * @param int value
 * 	 If value=1 then the page is writeable and readable. If value=0, then 
 * 	 the page is only readable, i.e., all mapped pages are readable.
 * @param int value
 */
void map(int process_id, int virtual_address, int value) {
	// TODO
}


/**
 * TODO
 *
 * @param
 * @param char instruction_type[]
 * 	one of {m, s, l}, where 
 * 	- m represents map,
 * 	- s represents store, and
 * 	- l represents load
 * @param
 * @param 
 */
void process_commmand(int process_id, char instruction_type, int virtual_address, int value) {
	// TODO Don't do this yet, put it off, see if there's any logic that needs to apply for all processes
}

/**
 * Parses the given string input, and handles it by subsequently calling process_command
 */
void parse_and_process_command(char *str[]) {
	// TODO
}

/**
 * Loops the read, evaluate, and print loop
 */
void loop_repl() {
	// TODO, probably copy logic from earlier project
}

int main() {
	printf("Hello world!");
}
