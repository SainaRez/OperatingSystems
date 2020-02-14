#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> // should always include errno when doing system calls to debug
#include <stdlib.h> // included for exit()
#include <sys/time.h>

#define PTBR_SIZE 4
#define MAX_MEMORY_SIZE 64
#define FREE_MEMORY_SIZE 4
#define PAGE_TABLE_ENTRY_NUM 4
#define PAGE_SIZE 16
#define STATUS_Empty 0
#define STATUS_USED_PRESENT 1
#define STATUS_USED_NOT_PRESENT 2

unsigned char memory[MAX_MEMORY_SIZE];

int page_table_base_register[PTBR_SIZE];

int available_memory[FREE_MEMORY_SIZE];

typedef struct Entry {
    
    unsigned char virtual_page;
    unsigned char physical_page;
    unsigned char protectionBits;
    unsigned char status;

} Entry;

typedef struct Page_Table {
    struct Entry entries[PAGE_TABLE_ENTRY_NUM];
} Page_Table;

typedef struct Page {
    unsigned char bytes[PAGE_SIZE];
} Page;

FILE *swap_space;

// Initializes the page_table_base_register (-1) and the availables memory arrays (zeros)
void initialize_arrays(){
    for(int i = 0; i < PTBR_SIZE; i++){
        page_table_base_register[i] = -1;
    }

    for(int i = 0; i < FREE_MEMORY_SIZE; i++){
        available_memory[i] = 0;
    }
    return;
}

// Checks if a memory is available and return the first index of the free memory. If none is available,
// it returns -1

int free_memory_index() {
    for (int i = 0; i < FREE_MEMORY_SIZE; i++) {
        if (available_memory[i] == 0) {
            available_memory[i] = 1;
            return i;
        }
        else {
            if (i == FREE_MEMORY_SIZE - 1) {
                // printf("No frame available");
                return -1;
            }
        }
    }
}

// Gets a page address between 0 and 63 and returns the page number. If address is in 
// range[0,15], page is 0. If address in in range [16,31], page is 1. If address is in 
// range [32,47], page is 2 and if address is in range [48,63], page is 3.

int get_virtual_page(int virtual_address) {
    int virtual_page;
    virtual_page = (int)(virtual_address / PAGE_SIZE);
    return virtual_page;
}

// Gets the virtual page
// Accesses the page table based on the memory address
// Looks for the entry that has that virtual page number

Entry* get_entry_of_virtual_page(int vir_page, int memory_address) {
    Page_Table *table = (Page_Table*) &memory[memory_address];
    for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
        if (table->entries[i].virtual_page == vir_page) {
            Entry *entry = &table->entries[i];
            return entry;
        }
    }
    return NULL;
}

// Checks if a given value in the givnen array
// It also takes the number of elements in the array

int valueinarray(int val, int arr[], int argc) {
	int i;
	for(i = 0; i < argc; i++) {
		if(arr[i] == val)
			return 1;
	}
	return 0;
}

// Kick out the first page that is not a table
int which_page_to_swap() {
    for (int i; i < PAGE_TABLE_ENTRY_NUM; i ++) {
        if (valueinarray((i * PAGE_SIZE),  page_table_base_register, PTBR_SIZE) == 0) {
            return i;
        }
        else {
            continue;
        }
    }
    printf("Error: All memory slots hold a page table\n");
    return -1;

}

int swap_page_table() {
    
}

int swap_to_disk() {
    
    swap_space = fopen("swap_space.txt", "wb");

    int page_index = which_page_to_swap();
    int page_address_in_memory = page_table_base_register[page_index * PAGE_SIZE];
    
    
    //fprintf(swap_space, "%d ", offset);  // Prints the line number
	//for (int i= 0; i < PAGE_SIZE; i++) {   
		//fprintf(swap_space, "%u ", memory[page_address_in_memory + i]);
   // }

    fwrite(&memory[page_address_in_memory], sizeof(char), PAGE_SIZE, swap_space);
    

    int offset = ftell(swap_space);
    fclose(swap_space);
    return offset;
    //int file_descriptor = fileno(swap_space);
    //lseek(f_stream, , )
}

void swap_back_to_memory(int offset, int available_memory_index) {
   
    swap_space = fopen("swap_space.txt", "rb");
    fseek(swap_space, offset, SEEK_SET);
    char* line;
    fgets(line, PAGE_SIZE, swap_space);
    
    int memory_start_address = available_memory_index * PAGE_SIZE;
    int i = 0;
    char page_array[PAGE_SIZE];
    char *token;
    token = strtok(line, " ");


    while (i < PAGE_SIZE) {
        memory[i + memory_start_address] = *token;
        token = strtok(NULL, " ");
    }
    return;

}

/**
 * Updates the page table entry for the given process and virtual_address to indicate that the
 * associated virtual_page is now located on swap space, located at swap_address
 *
 * @param pid
 * @param virtual_address
 * @param swap_address The address in swap space where the memory was copied to
 */
void update_page_table_for_swap_out(int pid, int virtual_address, int swap_address) {
    int virtual_page = get_virtual_page(virtual_address);
    Entry *page_table_entry = get_entry_of_virtual_page(virtual_page, page_table_base_register[pid]);
    if (page_table_entry == NULL) {
        printf("Error: Did not find a mapped memory for the given virtual address");
        return;
    }
    else {
        page_table_entry->physical_page = swap_address;
        page_table_entry->status = STATUS_USED_NOT_PRESENT;
    }
    return;
}

// void update_page_table_for_swap_in(int pid, int virtual_address, int offset) {
//     int virtual_page = get_virtual_page(virtual_address);
//     Entry *page_table_entry = get_entry_of_virtual_page(virtual_page, page_table_base_register[pid]);
//     if (page_table_entry == NULL) {
//         printf("Error: Did not find a mapped memory for the given virtual address");
//         return;
//     }
//     else {
//         page_table_entry->physical_page = offset;
//         page_table_entry->status = STATUS_USED_PRESENT;
//     }
//     return;
// }


// Maps the virtual memory to the physical memory
void map(int pid, int virtual_address, unsigned char value){
    int memory_index;
    if (page_table_base_register[pid] == -1) {  //it means that process isn't mapped at all
        //printf("Entered the no mapped process statement\n");
        memory_index = free_memory_index(); //get the index of first free memory
        if (memory_index == -1) {
            printf("Warning: No memory is available for the page table\n");
            printf("Swapping a page to file");
            int disk_address = swap_to_disk();
            update_page_table_for_swap_out(pid, virtual_address, disk_address);
        }
        else {
            // Create a page table at the memory address in page_table_base_register
            page_table_base_register[pid] = memory_index * PAGE_SIZE;
            printf("Put page table for PID %i into physical frame %i\n", pid, memory_index);
            
            // Now we check the memory again for the page that is being mapped to belong to that process (last time was page table)
            memory_index = free_memory_index();
            if (memory_index == -1) { 
                printf("No Memory is available for entry\n");
                int disk_address = swap_to_disk();
                update_page_table_for_swap_out(pid, virtual_address, disk_address);
            }
            else {
                int virtual_page = get_virtual_page(virtual_address);
                Entry page_table_entry = {virtual_page, memory_index, value, STATUS_USED_PRESENT};
               
                int memory_address = page_table_base_register[pid];
                Page_Table *table = (Page_Table*) &memory[memory_address];
                table->entries[0] = page_table_entry;

                printf("Mapped virtual address %i (page %i) into physical frame %i\n", virtual_address, virtual_page, memory_index);

                printf("printing the in use values\n");
                for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                    printf("Entry %i in use value: %i\n", i, table->entries[i].status);
                }

                printf("printing the in physical frame value:\n");
                for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                    printf("Entry %i physical frame value: %i\n", i, table->entries[i].physical_page);
                }
            }            
        }
    }
    else { //process is mapped before so we check for the specific virtual page
        //printf("Entered the already mapped process statement\n");
        memory_index = free_memory_index();
        if (memory_index == -1) {
            printf("Error: No memery available\n");
            int disk_address = swap_to_disk();
            update_page_table_for_swap_out(pid, virtual_address, disk_address);
                
            //return;
        }
        else {
            int virtual_page = get_virtual_page(virtual_address);
            Entry *page_table_entry = get_entry_of_virtual_page(virtual_page, page_table_base_register[pid]);
        
            if (page_table_entry == NULL) {
                Entry page_table_entry = {virtual_page, memory_index, value, STATUS_USED_PRESENT};
                int memory_address = page_table_base_register[pid];
                Page_Table *table = (Page_Table*) &memory[memory_address];

                // printf("printing the in use values\n");
                // for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                //     printf("Entry %i in use value: %i\n", i, table->entries[i].status);
                // }

                // printf("printing the in physical frame value:\n");
                // for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                //     printf("Entry %i physical frame value: %i\n", i, table->entries[i].physical_page);
                // }
                
                for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                    if (table->entries[i].status == 0) {
                        table->entries[i] = page_table_entry;
                        break;
                    }
                    else {
                        if (i == PAGE_TABLE_ENTRY_NUM - 1) {
                            printf("Error: All the entries are in use\n");
                            return;
                        }
                    }
                }
                printf("Mapped virtual address %i (page %i) into physical frame %i\n", virtual_address, virtual_page, memory_index);
            }
            else {
                printf("Warning: Memory already mapped with the given virtual address\n");
                if (page_table_entry->protectionBits != value) {
                    page_table_entry->protectionBits = value;
                    printf("Updating permissions for virtual page %i (frame %i)\n", virtual_page, memory_index);
                } 
                return;
            }
        }
    }
    printf("available_memory:\n");
    for (int i = 0; i < PTBR_SIZE; i++) {
        printf("%i ", available_memory[i]);
    }
    printf("\n");
    printf("page_table_base_register:\n");
    for (int i = 0; i < PTBR_SIZE; i++) {
        printf("%d ", page_table_base_register[i]);
    }
    printf("\n");

    return;  
}


// Instructs the memory manager to write the supplied value
// into the physical memory location associated with the provided
// virtual address
void store(int pid, int virtual_address, int value){
    //Frist check if process is mapped
    int memory_address = page_table_base_register[pid];
    if (memory_address == -1) {
        printf("Error: Memory is not mapped");
        return;
    }
    else {
        int virtual_page = get_virtual_page(virtual_address);
        Entry *entry = get_entry_of_virtual_page(virtual_page, memory_address);
        if (entry == NULL) {
            printf("Error: No memory with the given virtual address was found\n");
            return;
        }
        else {
            // Frist check if it has write acess
            if (entry->protectionBits == 0) {
                printf("Entry does not provide write access");
                return;
            }
            else {
                int frame = entry->physical_page;
                printf("Entry physical page: %i\n", frame);
                int offset = virtual_address % PAGE_SIZE;
                printf("offset: %i\n", offset);
                int physical_address = (frame * PAGE_SIZE) + offset;
        
                //value = memory[physical_address];
                memory[physical_address] = (unsigned char) value;
                printf("Stored value %i at virtual address %i (physical address %i)\n", value, virtual_address, physical_address);
            }
        }
    }
    printf("available_memory:\n");
    for (int i = 0; i < PTBR_SIZE; i++) {
        printf("%i ", available_memory[i]);
    }

    printf("\n");
    printf("page_table_base_register:\n");
    for (int i = 0; i < PTBR_SIZE; i++) {
        printf("%d ", page_table_base_register[i]);
    }
    printf("\n");

    return;
}

// Instructs the  memory manager to return the byte stored at the
// memory location specified by virtual_address
void load(int pid, int virtual_address){
    //Frist check if process is mapped
    int memory_address = page_table_base_register[pid];
    if (memory_address == -1) {
        printf("Error: Memory is not mapped");
        return;
    }
    else {
        int virtual_page = get_virtual_page(virtual_address);
        Entry *entry = get_entry_of_virtual_page(virtual_page, memory_address);
        if (entry == NULL) {
            printf("Error: No entry with the given virtual address was found\n");
            return;
        }
        else {
            int frame = entry->physical_page;
            int offset = virtual_address % PAGE_SIZE;
            int physical_address = (frame * PAGE_SIZE) + offset;
            printf("The value %i is at virtual address %i (physical address %i)\n", memory[physical_address], virtual_address, physical_address);
        }
    }
    return;
}

// Makes sure that the arguments are valid and calls the function that corresponds to the given command
void process_command(int pid, char *command, int vir_addrs, int value){

    if (pid < 0 || pid > 3) {
        printf("Error: PID out of range\n");
        return;
    }
    if (vir_addrs < 0 || vir_addrs > 63) {
        printf("Error: virtual address out of memory range\n");
        return;
    }
    

    if (strcmp(command, "map") == 0) {
        
        if (value < 0 || value > 1) {
            printf("value: %c\n", value);
            printf("Error: protection value not supported\n");
            return;
    }
        map(pid, vir_addrs, value);

    }
    else if (strcmp(command, "store") == 0) {
        
        store(pid, vir_addrs, value);
    }
    else if (strcmp(command, "load") == 0) {
        load(pid, vir_addrs);
    }
    else {
        printf("Command not supported\n");
    }
    return;
}

// Prints the Instruction prompt every time until the user stops the program
void loop_repl(int argc, char* argv[]){

    int value;
    int pid, vir_addrs;
    char command[7], exit_check[4];
    initialize_arrays();
    
    while(1){

        printf("Instruction? ");
        int arg_count = scanf("%i,%6[^,^\n],%i,%i", &pid, command, &vir_addrs, &value);
        if (arg_count != 4){
            printf("Wrong number of inputs");
            return;
        }
        printf("count: %d\n",  arg_count);

        process_command(pid, command, vir_addrs, value);
    }
    return;
    
}

void test_read_write_disc() {
    map(3, 17, 1);
    store(3, 16, 255);
    store(3, 31, 15);
    Page *page_to_move = (Page *) &memory[16];
}


int main(int argc, char* argv[]){

    
    //loop_repl(argc, argv);
    test_read_write_disc();
    
    return 0;
}