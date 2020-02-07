#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 64
#define PAGE_SIZE 16
#define PROMPT "Instruction? "
#define MAX_PROCESSES 4

unsigned char memory[SIZE];

/** 
 * Each process will have a simulated hardware register pointing to the start of
 * their respective page tables. These registers are simulated with an array
 * indexed by process id. Values are initialized to -1 when there is no page table
 * yet. (Initialization is done by initialize_register_array())
 **/
int page_table_register_array[MAX_PROCESSES];

/**
 * A list of which pages a free. Free pages have a value of false
 */
bool page_use_status_array[SIZE / PAGE_SIZE];


/** 
 * Initialize all page_table_registers to -1
 */
void initialize_register_array() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        page_table_register_array[i] = -1;
    }
}

/**
 * Prints the contents of the page_table_register_array
 */
void print_page_table() {
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        printf("PageTable location of process %i = %i\n", i, page_table_register_array[i]);
    }
}

/**
 * Prints the state of which pages are free
 */
void print_page_use_status() {
    for (int i = 0; i < SIZE / PAGE_SIZE; ++i) {
        printf("Page %i use status?: %s\n", i, page_use_status_array[i] ? "true" : "false");
    }
}


/**
 * Finds the next available page in memory.
 * @return The Page # of the page, such as 0,1,2, or 3.
 */
int next_free_page_frame_number() {
    for (int i = 0; i < SIZE / PAGE_SIZE; ++i) {
        if (page_use_status_array[i] == false) {
            return i;
        }
    }

    // TODO handle no free memory case gracefully, don't just exit
    fprintf(stderr, "Error: No available pages left");
    exit(EXIT_FAILURE);
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
 * Returns true if the given process has a page table initialized into memory
 */
bool does_process_have_page_file(int process_id) {
    return (page_table_register_array[process_id] != -1);
}

/**
 * @param process_id Int between 0 and 3 specifying which process
 * @param virtual_address
 *      An integer value in the range [0,63] specifying the virtual memory
 *      location for given process
 */
void load(int process_id, int virtual_address) {

}

/**
 * Store instructs the memory manager to write the supplied value into the physical
 * memory location associated with the provided virtual address, performing
 * translation and page swapping as necessary.
 *
 * @param process_id Int between 0 and 3 specifying which process
 * @param virtual_address
 *      An integer value in the range [0,63] specifying the virtual memory
 *      location for given process
 * @param value
 */
void store(int process_id, int virtual_address, int value) {

}


/**
 * Creates a page table for the given process_id in the next available frame of physical memory.
 * @param process_id
 */
void create_page_table_for_process(int process_id) {
    int next_free_frame = next_free_page_frame_number();
    page_use_status_array[next_free_frame] = true; // Marking the frame as used
    page_table_register_array[process_id] = next_free_frame * PAGE_SIZE; // Setting the register to point to new frame
    printf("Put page table for PID %i into physical frame %i\n", process_id, next_free_frame);
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
    if (does_process_have_page_file(process_id) == false) {
        create_page_table_for_process(process_id);
    }



    return;
}


/**
 * Validates the given parameters, and runs the appropriate function associated with the given
 * command/instruction_type.
 *
 * Will print to stderr and exit if invalid parameters are given.
 *
 * @param process_id Int between 0 and 3 specifying which process
 * @param char instruction_type
 * 	one of {m, s, l}, where 
 * 	- m represents map,
 * 	- s represents store, and
 * 	- l represents load
 * @param virtual_address
 *      An integer value in the range [0,63] specifying the virtual memory
 *      location for given process
 * @param int value
 *      An integer in the range [0,255].
 */
void process_command(int process_id, char instruction_type, int virtual_address, int value) {
    if (process_id < 0 || process_id > 3) {
        fprintf(stderr, "Error: Process ID %i is out of range", process_id);
        exit(EXIT_FAILURE);
    }

    if (virtual_address < 0 || virtual_address > 63) {
        fprintf(stderr, "Error: virtual address %i is out of range", virtual_address);
        exit(EXIT_FAILURE);
    }

    if (value < 0 || value > 255) {
        fprintf(stderr, "Error: virtual address %i is out of range", virtual_address);
        exit(EXIT_FAILURE);
    }

    if (instruction_type == 'm') {
        if (value > 1) {
            fprintf(stderr, "Error: Illegal value argument %i passed with command map", value);
            return;
        }
        map(process_id, virtual_address, value);
    }

    else if (instruction_type == 's') {
        store(process_id, virtual_address, value);
    }
    else if (instruction_type == 'l') {
        load(process_id, virtual_address);
    }
    else {
        fprintf(stderr, "Illegal instruction_type %c", instruction_type);
        exit(EXIT_FAILURE);
    }
}


/**
 * Loops the read, evaluate, and print loop
 */
void loop_repl() {

    while (true) {
        int pid, virtual_address, value;
        char command[8] = {'\0'};

        printf(PROMPT);

        // Source: https://stackoverflow.com/a/12910012/4225094
        int num_scanned_values = scanf("%i,%7[^,^\n],%i,%i", &pid, command, &virtual_address, &value);
        if (num_scanned_values != 4) {
            fprintf(stderr, "Illegal command input. Scanf could only read %i values instead of the expected 4",
                    num_scanned_values);
            exit(EXIT_FAILURE);
        }

        char command_char = '\0';
        if (strcmp(command, "map") == 0) {
            command_char = 'm';
        } else if (strcmp(command, "store") == 0) {
            command_char = 's';
        } else if (strcmp(command, "load") == 0) {
            command_char = 'l';
        } else {
            fprintf(stderr, "Command %s not recognized.", command);
            exit(EXIT_FAILURE);
        }

        process_command(pid, command_char, virtual_address, value);

        // fflush just to be safe.
        fflush(stdout);
        fflush(stdin);
    }
}

/**
 * A simple script that tests a series of map commands. Use from main to see if map is working.
 */
void test_map() {
    print_page_table();
    print_page_use_status();
    print_memory();

    process_command(0, 'm', 0, 0);
    process_command(0, 'm', 17, 1);
    process_command(0, 'm', 17, 0);
    process_command(2, 'm', 0, 0);
    process_command(1, 'm', 0, 0);
    process_command(3, 'm', 0, 1);
    print_page_table();
    print_page_use_status();
    print_memory();
}

int main() {
    initialize_register_array();

    test_map();

    loop_repl();
}
