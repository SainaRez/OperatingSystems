#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> // should always include errno when doing system calls to debug
#include <stdlib.h> // included for exit()
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define PTBR_SIZE 4
#define MAX_MEMORY_SIZE 64
#define FREE_MEMORY_SIZE 4
#define PAGE_TABLE_ENTRY_NUM 4
#define PAGE_SIZE 16

unsigned char memory[MAX_MEMORY_SIZE];

int page_table_base_register[PTBR_SIZE];

int available_memory[FREE_MEMORY_SIZE];

typedef struct Entry {
    
    unsigned char virtual_page;
    unsigned char physical_page;
    unsigned char protectionBits;
    unsigned char present;
    unsigned char in_use;

} Entry;

typedef struct Page_Table {
    struct Entry entries[PAGE_TABLE_ENTRY_NUM];
} Page_Table;

//FILE *swap_space;

void initialize_arrays(){
    for(int i = 0; i < PTBR_SIZE; i++){
        page_table_base_register[i] = -1;
    }

    for(int i = 0; i < FREE_MEMORY_SIZE; i++){
        available_memory[i] = 0;
    }
    return;
}

//checks if a memory is available and return the first index of the free memory. If none is available,
//it returns -1
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

// void swap() {
//     int page_index = which_page_to_swap();
//     int page_address_in_memory = page_table_base_register[page_index * PAGE_SIZE];
//     //long current_position = ftell(swap_space);
//     //printf("current_position: %ld", current_position);
//     //fwrite(&memory[page_address_in_memory], sizeof(char), PAGE_SIZE, swap_space);
    
    
//     //int file_descriptor = fileno(swap_space);
//     //lseek(f_stream, , )
// }




// Maps the virtual memory to the physical memory
void map(int pid, int virtual_address, unsigned char value){
    int memory_index;
    if (page_table_base_register[pid] == -1) {  //it means that process isn't mapped at all
        printf("Entered the no mapped process statement\n");
        memory_index = free_memory_index(); //get the index of first free memory
        if (memory_index == -1) {
            printf("Warning: No memory is available for the page table\n");
            printf("Swapping a page to file");
            //swap();
            return;
        }
        else {
            // Create a page table at the memory address in page_table_base_register
            page_table_base_register[pid] = memory_index * PAGE_SIZE;
            printf("Put page table for PID %i into physical frame %i\n", pid, memory_index);
            
            // Now we check the memory again for the page that is being mapped to belong to that process (last time was page table)
            memory_index = free_memory_index();
            if (memory_index == -1) { 
                printf("No Memory is available for entry\n");
            }
            else {
                int virtual_page = get_virtual_page(virtual_address);
                Entry page_table_entry = {virtual_page, memory_index, value, 1, 1};
               
                int memory_address = page_table_base_register[pid];
                Page_Table *table = (Page_Table*) &memory[memory_address];
                table->entries[0] = page_table_entry;

                printf("printing the in use values\n");
                for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                    printf("Entry %i in use value: %i\n", i, table->entries[i].in_use);
                }

                printf("printing the in physical frame value:\n");
                for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                    printf("Entry %i physical frame value: %i\n", i, table->entries[i].physical_page);
                }

                
                // Page_Table *new_table;
                // new_table->entries[0] = page_table_entry;
                
                // int memory_address = page_table_base_register[pid];
                // new_table = (Page_Table *) &memory[memory_address];
                // //(Page_Table *) &memory[memory_address] = &new_table;

            }            
        }
    }
    // Is it possible to initialize the free variable so we just check that?
    else { //process is mapped before so we check for the specific virtual page
        printf("Entered the already mapped process statement\n");
        memory_index = free_memory_index();
        if (memory_index == -1) {
            printf("Error: No memery available\n");
            return;
        }
        else {
            int virtual_page = get_virtual_page(virtual_address);
            Entry *page_table_entry = get_entry_of_virtual_page(virtual_page, page_table_base_register[pid]);
        
            if (page_table_entry == NULL) {
                Entry page_table_entry = {virtual_page, memory_index, value, 1, 1};
                int memory_address = page_table_base_register[pid];
                Page_Table *table = (Page_Table*) &memory[memory_address];

                printf("printing the in use values\n");
                for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                    printf("Entry %i in use value: %i\n", i, table->entries[i].in_use);
                }

                printf("printing the in physical frame value:\n");
                for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                    printf("Entry %i physical frame value: %i\n", i, table->entries[i].physical_page);
                }
                
                for (int i = 0; i < PAGE_TABLE_ENTRY_NUM; i++) {
                    if (table->entries[i].in_use == 0) {
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
//into the physical memory location associated with the provided
//virtual address
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
//memory location specified by virtual_address
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

void loop_repl(int argc, char* argv[]){

    int value;
    int pid, vir_addrs;
    char command[7], exit_check[4];
    initialize_arrays();

    //FILE *swap_space;
    //swap_space = fopen("swap_space.txt", "w+");

    
    while(1){

        printf("Instruction? ");
        int arg_count = scanf("%i,%6[^,^\n],%i,%i", &pid, command, &vir_addrs, &value);
        if (arg_count != 4){
            printf("Wrong number of inputs");
            return;
        }
        printf("count: %d\n",  arg_count);

        process_command(pid, command, vir_addrs, value);

        // printf("Are you done? (type yes or no) \n");
        // scanf("%s", exit_check);

        // if (strcmp(exit_check, "yes") == 0){
        //     exit(1);
        //}
    }
    return;
    
}


int main(int argc, char* argv[]){

    
    loop_repl(argc, argv);
    
    return 0;
}