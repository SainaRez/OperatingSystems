#include <stdio.h>
#include <stdbool.h>

#define SIZE 64
#define PAGE_SIZE 16
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


/** 
 * Initialize all page_table_registers to -1
 */
void initialize_register_array() {
    for (int i = 0; i < 4; i++) {
        page_table_register_array[i] = -1;
    }
}


/**
 * Utility function to print the entire contents of memory
 */
void print_memory() {
    for (int y = 0; y < SIZE / 16; y++) {
        for (int x = 0; x < 16; x++) {
            printf("0x%04x, ", memory[16 * y + x]);
        }
        printf("\n");
    }

    printf("\n");
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
    // TODO this method can include logic for parameter validation (since
    // much of it is the same accross all the commands
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

    if (3 < process_id && process_id < 0) {
        printf("process ID is out of range");
    }


}


/**
 * Loops the read, evaluate, and print loop
 */
void loop_repl() {
    int pid, vir_addrs, value;
    char command[512], exit_check[512];

    while (1) {

        printf("Instruction?  ");
        scanf("%i,%s,%i,%i\n", &pid, command, &vir_addrs, &value);

        printf("Are you done? (type yes or no) \n");
        scanf("%s", exit_check);

        if (strcmp(exit_check, "yes") == 0) {
            exit(1);
        }
    }
}

int main() {
    initialize_register_array();
    print_memory();

    unsigned char foo = 65;
    unsigned char bar = 66;
    memory[16] = foo;
    memory[17] = bar;

    print_memory();

}
