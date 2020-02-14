#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SIZE 64
#define PAGE_SIZE 16
#define PROMPT "Instruction? "
#define MAX_PROCESSES 4

unsigned char memory[SIZE];
const bool verbose = true;

/**
 * An Entry is a single mapping of a virtual page to a location in swap or memory.
 */
typedef struct Entry {
    unsigned char is_used; // == 1 if the entry is populated
    unsigned char virtual_page;
    unsigned char physical_page;
    unsigned char writable;
} Entry;

/**
 * Any page in memory can be referred to with this struct
 */
typedef struct Page {
    unsigned char bytes[PAGE_SIZE];
} Page;

#define ENTRIES_PER_PAGE_TABLE 4
/**
 * A Page_Table is a specific type of page that stores mappings of virtual memory
 * to physical memory for processes.
 */
typedef struct Page_Table {
    struct Entry entries[ENTRIES_PER_PAGE_TABLE];
} Page_Table;

FILE *swap_space;

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
 * Utility function to print the entire contents of memory
 */
void print_memory() {
    for (int y = 0; y < SIZE / 16; y++) {
        for (int x = 0; x < 16; x++) {
            printf("0x%02x, ", memory[16 * y + x]);
            // Use below to print in integer form instead of hexadecimal
            // printf("%03i, ", memory[16 * y + x]);
        }
        printf("\n");
    }
    printf("\n");
}


/**
 * Returns true if there is still at least on available free page.
 */
bool check_free_pages() {
    for (int i = 0; i < SIZE / PAGE_SIZE; ++i) {
        if (page_use_status_array[i] == false) {
            return true;
        }
    }

    return false;
}


/**
 * Finds the next available page in memory. There must be memory free, or else this method exits. Use check_free_pages
 * to determine if there is a free page.
 *
 * @see check_free_pages()
 *
 * @return The Page # of the page, such as 0,1,2, or 3.
 */
int next_free_page_frame_number() {
    for (int i = 0; i < SIZE / PAGE_SIZE; ++i) {
        if (page_use_status_array[i] == false) {
            return i;
        }
    }

    fprintf(stderr, "Error: No available pages left\n");
    exit(EXIT_FAILURE);
}


/**
 * Returns true if the given process has a page table initialized into memory
 */
bool does_process_have_page_file(const int process_id) {
    return (page_table_register_array[process_id] != -1);
}


/**
 * Converts a virtual address into a virtual page number.
 * {0..15} -> 0
 * {16..31} -> 1
 * {32..47} -> 2
 * {48..63} -> 3
 *
 * @param virtual_address (an int in the range [0,63])
 * @return The virtual page number, an int in the range 0,3
 */
int get_virtual_page_of_address(const int virtual_address) {
    return (virtual_address - (virtual_address % PAGE_SIZE)) / PAGE_SIZE;
}

/**
 * Returns a pointer to the Entry where the virtual page mapping is located in memory.
 *
 * get_physical_page_of_virtual will return NULL if given a virtual_page not mapped to physical memory.
 *
 * @param process_id int between 0 and 3 specifying which process
 * @param virtual_page Int between 0 and 3 specifying which virtual page to look for
 * @return The entry that maps virtual_page to a physical page, or NULL
 */
Entry *get_entry_of_virtual_page(const int process_id, const int virtual_page) {
    const int page_table_address = page_table_register_array[process_id];
    assert(page_table_address % PAGE_SIZE == 0);

    Page_Table *page_table = (Page_Table *) &memory[page_table_address];

    for (int i = 0; i < ENTRIES_PER_PAGE_TABLE; ++i) {
        Entry *entry = &page_table->entries[i];
        if (entry->is_used == false) {
            continue;
        }
        if (entry->virtual_page == virtual_page) {
            return entry;
        }
    }

    return NULL;
}

// Checks if a given value in the givnen array
// It also takes the number of elements in the array

int valueinarray(int val, int arr[], int argc) {
    int i;
    for (i = 0; i < argc; i++) {
        if (arr[i] == val)
            return 1;
    }
    return 0;
}

// Kick out the first page that is not a table
int which_page_to_swap() {
    for (int i; i < ENTRIES_PER_PAGE_TABLE; i++) {
        if (valueinarray((i * PAGE_SIZE), page_table_register_array, MAX_PROCESSES) == 0) {
            return i;
        } else {
            continue;
        }
    }
    //printf("Error: All memory slots hold a page table\n");
    return -1;
}


/**
 * @param process_id Int between 0 and 3 specifying which process
 * @param virtual_address
 *      An integer value in the range [0,63] specifying the virtual memory
 *      location for given process
 */
void load(const int process_id, const int virtual_address) {
    if (does_process_have_page_file(process_id) == false) {
        fprintf(stderr, "Error: No page table set up for process %i\n", process_id);
        return;
    }

    const int virtual_page = get_virtual_page_of_address(virtual_address);

    Entry *page_table_entry = get_entry_of_virtual_page(process_id, virtual_page);
    if (page_table_entry == NULL) {
        fprintf(stderr, "Error: No mapping found for given virtual address %i\n", virtual_address);
        return;
    }

    const int physical_page = page_table_entry->physical_page;

    const int offset = virtual_address % PAGE_SIZE;
    const int physical_address = PAGE_SIZE * physical_page + offset;

    printf("The value %i is at virtual address %i\n", memory[physical_address], virtual_address);
}


/**
 * Store instructs the memory manager to write the supplied value into the physical
 * memory location associated with the provided virtual address, performing
 * translation and page swapping as necessary.
 *
 *
 * @param process_id Int between 0 and 3 specifying which process
 * @param virtual_address
 *      An integer value in the range [0,63] specifying the virtual memory
 *      location for given process
 * @param value
 *      An integer value in the range [0,255] specifying the value to set in memory
 */
void store(const int process_id, const int virtual_address, const int value) {
    if (does_process_have_page_file(process_id) == false) {
        fprintf(stderr, "Error: No page table set up for process %i\n", process_id);
        return;
    }

    const int virtual_page = get_virtual_page_of_address(virtual_address);

    Entry *page_table_entry = get_entry_of_virtual_page(process_id, virtual_page);

    if (page_table_entry == NULL) {
        fprintf(stderr, "Error: No mapping found for given virtual address %i\n", virtual_address);
        return;
    }
    if (page_table_entry->writable == false) {
        printf("Error: writes are not allowed to this page\n");
        return;
    }
    const int physical_page_frame = page_table_entry->physical_page;

    const int offset = virtual_address % PAGE_SIZE;
    const int physical_address = physical_page_frame * PAGE_SIZE + offset;
    memory[physical_address] = (unsigned char) value;

    printf("Stored value %i at virtual address %i (physical address %i)\n", value, virtual_address, physical_address);
}


/**
 * Creates a page table for the given process_id in the next available frame of physical memory.
 * @param process_id
 */
void create_page_table_for_process(const int process_id) {
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
void map(const int process_id, const int virtual_address, const int value) {
    if (does_process_have_page_file(process_id) == false) {
        if (check_free_pages() == false) { // TODO, should this check for 2 free pages? Ask TA
            printf("Error: No more free pages in memory.\n");
            //swap();
        }
        create_page_table_for_process(process_id);
    }

    int virtual_page = get_virtual_page_of_address(virtual_address);
    Entry *page_table_entry = get_entry_of_virtual_page(process_id, virtual_page);
    if (page_table_entry != NULL) {
        if (page_table_entry->writable == value) {
            printf("Error: virtual page %i is already mapped with rw_bit=%i\n", virtual_page,
                   page_table_entry->writable);
        } else {
            page_table_entry->writable = value;
            printf("Updating permissions for virtual page %i (frame %i)\n", virtual_page,
                   page_table_entry->physical_page);
        }
        return;
    }

    // Else, there is no existing mapping, so create one if there is space
    if (check_free_pages() == false) {
        printf("Error: No more free pages in memory.\n");
        // swap();
        //return; // TODO See related todo above, should this undo the page_table that was created?
    }

    const int physical_page_frame = next_free_page_frame_number();
    page_use_status_array[physical_page_frame] = true; // mark the frame as in use

    Page_Table *page_table = (Page_Table *) &memory[page_table_register_array[process_id]];
    for (int i = 0; i < ENTRIES_PER_PAGE_TABLE; ++i) {
        if (page_table->entries[i].is_used == false) {
            Entry *new_map_entry = &page_table->entries[i];
            new_map_entry->is_used = true;
            new_map_entry->virtual_page = virtual_page;
            new_map_entry->physical_page = physical_page_frame;
            new_map_entry->writable = value;
            break;
        }
    }

    printf("Mapped virtual address %i (page %i) into physical frame %i\n", virtual_address, virtual_page,
           physical_page_frame);
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
void process_command(const int process_id, const char instruction_type, const int virtual_address, const int value) {
    if (process_id < 0 || process_id > 3) {
        fprintf(stderr, "Error: Process ID %i is out of range\n", process_id);
        exit(EXIT_FAILURE);
    }

    if (virtual_address < 0 || virtual_address > 63) {
        fprintf(stderr, "Error: virtual address %i is out of range\n", virtual_address);
        exit(EXIT_FAILURE);
    }

    if (value < 0 || value > 255) {
        fprintf(stderr, "Error: given value %i is out of range\n", value);
        exit(EXIT_FAILURE);
    }

    if (instruction_type == 'm') {
        if (value > 1) {
            fprintf(stderr, "Error: Illegal value argument %i passed with command map\n", value);
            return;
        }
        map(process_id, virtual_address, value);
    } else if (instruction_type == 's') {
        store(process_id, virtual_address, value);
    } else if (instruction_type == 'l') {
        load(process_id, virtual_address);
    } else {
        fprintf(stderr, "Illegal instruction_type %c\n", instruction_type);
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
            fprintf(stderr, "Illegal command input. Scanf could only read %i values instead of the expected 4\n",
                    num_scanned_values);
            exit(EXIT_FAILURE);
        }

        char command_char;
        if (strcmp(command, "map") == 0) {
            command_char = 'm';
        } else if (strcmp(command, "store") == 0) {
            command_char = 's';
        } else if (strcmp(command, "load") == 0) {
            command_char = 'l';
        } else {
            fprintf(stderr, "Command %s not recognized.\n", command);
            exit(EXIT_FAILURE);
        }

        process_command(pid, command_char, virtual_address, value);

        // fflush just to be safe.
        fflush(stdout);
        fflush(stdin);
    }
}

/**
 * test_easy_extended performs some commands that won't cause any errors or deviate from standard behavior.
 */
void test_easy_extended() {
    map(3, 25, 1);
    map(3, 55, 1);
    store(3, 30, 255);
    store(3, 31, 127);
    store(3, 17, 16);
    load(3, 30);
    load(3, 31);
    load(3, 17);
    store(3, 49, 77);
    load(3, 49);

    print_memory();
}

/**
 * A quick test that checks to see if map, store, and load in the simplest use case.
 */
void test_easy() {
    map(0, 0, 1);
    store(0, 12, 24);
    load(0, 12);
}

/**
 * Prints the contents of the swap file, if it exists.
 */
void print_swap() {
    FILE *swap;
    fopen_s(&swap, "swap_space.bin", "rb");
    if (swap == NULL) { return; }

    int i = 0;
    int byte;
    while ((byte = getc(swap)) != EOF) {
        if (i % PAGE_SIZE == 0) {
            printf("\n");
        }
        i++;
        printf("%02x,", byte);
    }
    fclose(swap);
    printf("EOF\n");
}

void print_page(Page *page) {
    assert(page != NULL);
    for (int i = 0; i < PAGE_SIZE; ++i) {
        printf("%02x", page->bytes[i]);
    }
    printf("\n");
}

/**
 * Copies the contents of the swap space located at swap_address into the simulated memory
 * at the given address
 * @param swap_address Memory address in swap space.
 * @param physical_memory_address Memory address in physical space
 */
void copy_swap_page_to_memory(int swap_address, int physical_memory_address) {
    assert(swap_address % 16 == 0);
    assert(physical_memory_address % 16 == 0);

    FILE *swap = fopen("swap_space.bin", "rb");
    if (swap == NULL) {
        fprintf(stderr, "Error: Failed to open swap_space.\n");
        exit(EXIT_FAILURE);
    }

    Page *physical_page = (Page *) &memory[physical_memory_address];
    fread(physical_page, sizeof(Page), 1, swap);

    print_page(physical_page);

    fclose(swap);
}

/**
 * Copies the contents of the memory at location page, and writes it to the swap file at location swap_address.
 * @param page A pointer to the page in memory whose contents are to be copied
 * @param swap_address The address in swap space memory where the contents are to be written
 */
void copy_memory_page_to_disc(Page *page, int swap_address) {
    FILE *swap = fopen("swap_space.bin", "wb");
    fwrite(page, sizeof(Page), PAGE_SIZE, swap);
    fclose(swap);
}

/**
 * Clears the contents of the swap file
 */
void clear_swap() {
    remove("swap_space.bin");
}

void test_read_write_disc() {
    map(3, 17, 1);
    store(3, 16, 255);
    store(3, 31, 15);
    // Memory now contains 255,0,0... in page 1 or memory[16]
    Page *page_to_move = (Page *) &memory[16];
    copy_memory_page_to_disc(page_to_move, 0);
    // Memory should not contain anything in page 1
    print_swap();
    copy_swap_page_to_memory(0, 48);
    // Memory should contain 255,0,0... in page 3 or memory[48]
    print_memory();
}

int main(int argc, char *argv[]) {
    initialize_register_array();
    clear_swap();

    test_read_write_disc();
    //test_easy();
    //test_easy_extended();

    loop_repl();
}
