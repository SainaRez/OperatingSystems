#include <stdio.h>
#include <stdbool.h>

#define SIZE 64
#define PAGE_SIZE 16
#define PROMPT "Instruction? "
#define MAX_PROCESSES 4

unsigned char memory[SIZE];

/** 
 * Each process will have a simulated hardware register pointing to the start of
 * their respective page tables. These registers are simulated with an array
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
 * Prints the contents of the page_table_register_array
 */
void print_page_table() {
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        printf("PageTable[%i] = %i\n", i, page_table_register_array[i]);
    }
}


/**
 * Utility function to print the entire contents of memory
 */
void print_memory() {
    for (int y = 0; y < SIZE / 16; y++) {
        for (int x = 0; x < 16; x++) {
            printf("0x%02x, ", memory[16 * y + x]);
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
 * @param process_id Int between 0 and 3 specifying which process
 * @param virtual_address
 *      An integer value in the range [0,63] specifying the virtual memory
 *      location for givenprocess
 */
int load(int process_id, int virtual_address) {

}

/**
 * Store instructs the memory manager to write the supplied value into the physical
 * memory location associated with the provided virtual address, performing
 * translation and page swapping as necessary.
 *
 * @param process_id Int between 0 and 3 specifying which process
 * @param virtual_address
 *      An integer value in the range [0,63] specifying the virtual memory
 *      location for givenprocess
 * @param value
 */
void store(int process_id, int virtual_address, int value) {

}


/**
 * Tells the memory manager to allocate a physical page, i.e., create a mapping
 * in the page table between a virtual and physical address. The manager must determine
 * the appropriate virtual page number using the virtual_address. For example, a virtual_address
 * of 3 corresponds to virtual page 0.
 *
 * Permissions can remapped after being set by subsequent map commands.
 *
 * @param process_id Int between 0 and 3 specifying which process
 * @param virtual_address
 *      An integer value in the range [0,63] specifying the virtual memory
 *      location for given process
 * @param int value
 * 	 If value=1 then the page is writeable and readable. If value=0, then 
 * 	 the page is only readable, i.e., all mapped pages are readable.
 */
void map(int process_id, int virtual_address, int value) {
    int virtual_page = (virtual_address - (virtual_address % PAGE_SIZE)) / PAGE_SIZE;


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
    print_page_table();
    map(0, 17, 0);


    // Below is just a demo of memory printing
    print_memory(); // Before

    unsigned char foo = 255;
    unsigned char bar = 0x65;
    memory[16] = foo;
    memory[17] = bar;

    print_memory(); // After
}
