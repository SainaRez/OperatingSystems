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

#define ENTRY_STATUS_EMPTY 0
#define ENTRY_STATUS_PRESENT 1
#define ENTRY_STATUS_SWAPPED 2


/**
 * An Entry is a single mapping of a virtual page to a location in swap or memory.
 */
typedef struct Entry {
    unsigned char status; // == 1 if the entry is populated
    unsigned char virtual_page;
    unsigned char physical_page; // Page Frame number, not address
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


#define REGISTER_SWAPPED -2
#define REGISTER_EMPTY -1
/**
 * Each process will have a simulated hardware register pointing to the start of
 * their respective page tables. These registers are simulated with an array
 * indexed by process id. Values are initialized to -1 when there is no page table
 * yet. (Initialization is done by initialize_register_array())
 **/
int page_table_register_array[MAX_PROCESSES];
int swapped_page_table_register_array[MAX_PROCESSES];

/**
 * A list of which pages a free. Free pages have a value of false
 */
bool page_use_status_array[SIZE / PAGE_SIZE];


/**
 * Initialize all page_table_registers to -1
 */
void initialize_register_array() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        page_table_register_array[i] = REGISTER_EMPTY;
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
 * Prints the contents of a page. The page can be a page_table or just some memory.
 * @param page A pointer to a Page in memory.
 */
void print_page(Page *page) {
    assert(page != NULL);
    for (int i = 0; i < PAGE_SIZE; ++i) {
        printf("%02x", page->bytes[i]);
    }
    printf("\n");
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
 * Prints the contents of the swap file, if it exists.
 */
void print_swap() {
    FILE *swap = fopen("swap_space.bin", "rb");
    if (swap == NULL) { return; }

    int i = 0;
    int byte;
    while ((byte = getc(swap)) != EOF) {
        if (i % PAGE_SIZE * 2 == 0) {
            printf("\n");
        }
        i++;
        printf("%02x,", byte);
    }
    fclose(swap);
    printf("EOF\n");
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
    return (page_table_register_array[process_id] != REGISTER_EMPTY);
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
        if (entry->status == ENTRY_STATUS_EMPTY) {
            continue;
        }
        if (entry->virtual_page == virtual_page) {
            return entry;
        }
    }

    return NULL;
}

/**
 * Returns true if the given memory address is a page table or not.
 * @param memory_address A address, such as 0, 16, 32, or 48, in simulated physical memory.
 * @return True if there is a page table stored at that address.
 */
bool is_page_table(int memory_address) {
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (page_table_register_array[i] == memory_address) {
            return true;
        }
    }
    return false;
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

    // Open file
    FILE *swap = fopen("swap_space.bin", "rb");
    if (swap == NULL) {
        fprintf(stderr, "Error: Failed to open swap_space.\n");
        exit(EXIT_FAILURE);
    }

    // Seek to address
    fseek(swap, swap_address, SEEK_SET);

    // Read contents and store in memory[]
    Page *physical_page = (Page *) &memory[physical_memory_address];
    fread(physical_page, sizeof(Page), 1, swap);

    fclose(swap);
}


/**
 * Copies the contents of the memory at location page, and writes it to the swap file at location swap_address.
 * @param page A pointer to the page in memory whose contents are to be copied
 * @param swap_address The address in swap space memory where the contents are to be written
 */
void copy_memory_page_to_disc(Page *page, int swap_address) {
    assert(swap_address % PAGE_SIZE == 0);
    assert(page != NULL);

    FILE *swap = fopen("swap_space.bin", "r+b");
    fseek(swap, swap_address, SEEK_SET);
    fwrite(page, sizeof(Page), 1, swap);
    fclose(swap);
}


/**
 * Determines which page should be swapped to disc. Prioritizes not swapping page_tables, or
 * memory related to the given process_id. Should not be called when there are still free
 * pages
 *
 * @see check_free_pages()
 *
 * @param process_id The process id which is causing a swap.
 * @return An address, such as 0, 16, 32, or 48.
 */
int which_page_to_swap(int process_id) {
    assert(check_free_pages() == false);

    // For each page in memory
    for (int i = 0; i < SIZE; i = i + PAGE_SIZE) {
        if (is_page_table(i) == false) {
            return i;
        }
    }

    // There are no pages that aren't page tables, so let's pick a page table to swap
    const int relevantPageTableAddress = page_table_register_array[process_id];
    for (int i = 0; i < SIZE; i = i + PAGE_SIZE) {
        if (relevantPageTableAddress == i) {
            continue;
        }

        return i;
    }

    fprintf(stderr, "Error: reached unreachable code.\n");
    exit(EXIT_FAILURE);
}


/**
 * Updates the given page table entry to indicate that the associated virtual page
 * is now located on swap space, located at swap_address
 *
 * @param entry a pointer to the entry for which memory is being swapped
 * @param swap_address The address in swap space where the memory was copied to
 */
void update_page_table_for_swap_out(Entry *entry, int swap_address) {
    assert(entry != NULL);

    entry->physical_page = swap_address / PAGE_SIZE;
    entry->status = ENTRY_STATUS_SWAPPED;
}


/**
 * Returns the associated page_table_entry for the given physical address.
 * The given address must be an address that is mapped to by a process.
 *
 * @param physical_address Address such as 0, 16, 32, or 48
 * @return A pointer to the Page_Table Entry which contains a mapping to the physical address
 */
Entry *get_page_table_entry_of_address(const int physical_address) {
    assert(physical_address % 16 == 0);
    const int physical_page = physical_address / 16;

    for (int a = 0; a < SIZE; a = a + PAGE_SIZE) {
        if (is_page_table(a) == false) {
            continue;
        }
        for (int x = 0; x < ENTRIES_PER_PAGE_TABLE; ++x) {
            Entry *entry = (Entry *) &memory[a + x * sizeof(Entry)];
            if (entry->status == ENTRY_STATUS_PRESENT &&
                entry->physical_page == physical_page) {
                return entry;
            }
        }
    }

    fprintf(stderr, "Error: No entry found for given address %i\n", physical_address);
    exit(EXIT_FAILURE);
}


/**
 * Empties the contents of memory at the given address to all 0's
 * @param page_address Address such as 0, 16, 32, 48
 */
void clear_physical_page(const int page_address) {
    assert(page_address % 16 == 0);
    Page *page = (Page *) &memory[page_address];
    for (int i = 0; i < PAGE_SIZE; ++i) {
        page->bytes[i] = 0;
    }
}


int swap_counter = 0; /** Increments to keep track of latest free space in memory */


/**
 * swap() will select a page of memory to be swapped to disc, and execute that swap.
 *
 * @param process_id The process id which is causing a swap
 * @return The page_address which is now free
 */
int swap(const int process_id) {
    // Figure out which page to swap
    const int page_address = which_page_to_swap(process_id);
    const int frame_number = page_address / PAGE_SIZE;
    const bool page_table = is_page_table(page_address);
    Page *page = (Page *) &memory[page_address];

    // Figure out where to swap the page to, and copy the memory there.
    const int swap_address = PAGE_SIZE * swap_counter++;
    copy_memory_page_to_disc(page, swap_address);

    // Mark the page moved as now being free;
    page_use_status_array[frame_number] = false;

    printf("Swapped frame %i to disc at swap slot %i\n", frame_number, swap_address / PAGE_SIZE);

    if (page_table == false) {
        // If it's not a page table, update the associated page table entry of whatever memory just got moved
        Entry *entry = get_page_table_entry_of_address(page_address);
        update_page_table_for_swap_out(entry, swap_address);
    } else if (page_table == true) {
        // If it was a page table, indicate that it's moved in the registers
        for (int i = 0; i < MAX_PROCESSES; ++i) {
            if (page_table_register_array[i] == page_address) {
                page_table_register_array[i] = REGISTER_SWAPPED;
                swapped_page_table_register_array[i] = swap_address;
            }
        }
    }

    clear_physical_page(page_address);
    return page_address;
}


/**
 * Checks to see if a page table is in memory for the process, and loads it into memory
 * from swap if it isn't.
 */
void prepare_page_table(int process_id) {
    assert(does_process_have_page_file(process_id));

    if (page_table_register_array[process_id] == REGISTER_SWAPPED) {
        // If the page_table is on swap space, move it back to memory
        const int swap_address_of_page_table = swapped_page_table_register_array[process_id];
        const int newly_free_physical_address = swap(process_id);
        assert(page_use_status_array[newly_free_physical_address / 16] == false);
        copy_swap_page_to_memory(swap_address_of_page_table, newly_free_physical_address);
        page_use_status_array[newly_free_physical_address / 16] = true;

        printf("Put page table for PID %i into physical frame %i\n", process_id, newly_free_physical_address);
        swapped_page_table_register_array[process_id] = 0;
        page_table_register_array[process_id] = newly_free_physical_address;
    }
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
    prepare_page_table(process_id);

    const int virtual_page = get_virtual_page_of_address(virtual_address);

    Entry *page_table_entry = get_entry_of_virtual_page(process_id, virtual_page);
    if (page_table_entry == NULL) {
        printf("Error: No mapping found for given virtual address %i\n", virtual_address);
        return;
    }

    if (page_table_entry->status == ENTRY_STATUS_SWAPPED) {
        // If the page_table is on swap space, move it back to memory and update page_table
        const int swap_address_of_page = page_table_entry->physical_page * 16;
        const int newly_free_physical_address = swap(process_id);

        // TODO duplicate code with store(), can probably be refactored
        printf("Swapped disk slot %i into frame %i\n", swap_address_of_page,
               newly_free_physical_address / PAGE_SIZE);

        copy_swap_page_to_memory(swap_address_of_page, newly_free_physical_address);
        page_use_status_array[newly_free_physical_address / PAGE_SIZE] = true;
        page_table_entry->status = ENTRY_STATUS_PRESENT;
        page_table_entry->physical_page = newly_free_physical_address / 16;
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
    prepare_page_table(process_id);

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
    if (page_table_entry->status == ENTRY_STATUS_SWAPPED) {
        // If the page is on swap space, move it back to memory and update page_table
        const int swap_address_of_page = page_table_entry->physical_page * PAGE_SIZE;
        const int newly_free_physical_address = swap(process_id);

        printf("Swapped disk slot %i into frame %i\n", swap_address_of_page,
               newly_free_physical_address / PAGE_SIZE);

        copy_swap_page_to_memory(swap_address_of_page, newly_free_physical_address);
        page_use_status_array[newly_free_physical_address / PAGE_SIZE] = true;
        page_table_entry->status = ENTRY_STATUS_PRESENT;
        page_table_entry->physical_page = newly_free_physical_address / PAGE_SIZE;
        assert(page_use_status_array[newly_free_physical_address / PAGE_SIZE]);
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
        if (check_free_pages() == false) {
            swap(process_id);
        }
        create_page_table_for_process(process_id);
    } else {
        // If there already is a page_table, make sure it is not in swap space.
        prepare_page_table(process_id);
    }

    // Get the entry of the virtual page from the page table, if it exists.
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
        swap(process_id);
    }

    const int physical_page_frame = next_free_page_frame_number();
    page_use_status_array[physical_page_frame] = true; // mark the frame as in use

    Page_Table *page_table = (Page_Table *) &memory[page_table_register_array[process_id]];
    for (int i = 0; i < ENTRIES_PER_PAGE_TABLE; ++i) {
        if (page_table->entries[i].status == false) {
            Entry *new_map_entry = &page_table->entries[i];
            new_map_entry->status = ENTRY_STATUS_PRESENT;
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


void test_swap() {
    map(0, 0, 1);
    store(0, 0, 255);
    map(0, 16, 1);
    store(0, 16, 254);
    map(0, 32, 1);
    store(0, 32, 253);
    map(0, 48, 1);
    store(0, 48, 252);
    map(1, 0, 1);
    store(1, 1, 255);
    map(1, 16, 1);
    store(1, 17, 254);
    map(1, 32, 1);
    store(1, 33, 253);
    map(1, 48, 1);
    store(1, 49, 252);
    map(2, 0, 1);
    store(2, 2, 15);
    map(2, 16, 1);
    store(2, 18, 14);
    map(2, 32, 1);
    store(2, 34, 13);
    map(2, 48, 1);
    store(2, 50, 12);
    map(3, 0, 1);
    store(3, 4, 255);
    map(3, 16, 1);
    store(3, 20, 255);
    map(3, 32, 1);
    store(3, 36, 255);
    map(3, 48, 1);
    store(3, 52, 255);
    load(0, 48);
    load(0, 32);
    load(0, 16);
    load(0, 0);
    load(2, 18);
    load(2, 2);
    load(2, 34);
    load(2, 50);
    load(3, 52);
    load(3, 36);
    load(3, 20);
    load(3, 4);
    load(1, 1);
    load(1, 17);
    load(1, 33);
    load(1, 49);
    map(2, 20, 0);
    map(2, 10, 0);
    load(2, 18);
    store(2, 25, 170);
    map(2, 20, 1);
    store(2, 25, 170);
    load(2, 25);
    load(2, 18);
    load(0, 16);
    load(3, 4);
    load(1, 33);
    map(0,0,0);
    map(1,0,0);
    map(2,0,0);
    map(3,0,0);
    load(0,16);
    // TODO copy (and reformat) this into a final_test.txt file or something
}

int main(int argc, char *argv[]) {
    initialize_register_array();
    remove("swap_space.bin");
    FILE *temp = fopen("swap_space.bin", "w");
    fclose(temp);

    // test_swap();

    loop_repl();
}
