#include <stdio.h>

#define SIZE 128
#define PROMPT "Instruction? "


/**
 * TODO
 *
 * @param int process_id
 * @param int value
 * 	 If value=1 then the page is writeable and readable. If value=0, then 
 * 	 the page is only readable, i.e., all mapped pages are readable.
 * @param int value
 */
void map(int process_id, itn virtual_address, int value) {
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
	// TODO
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
