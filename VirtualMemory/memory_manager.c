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
    unsigned char protectionBits;
    //unsigned char is_used;
    unsigned char virtual_page;
    unsigned char physical_page;
    unsigned char present;
} Entry;

typedef struct Page_Table {
    struct Entry entries[PAGE_TABLE_ENTRY_NUM];
} Page_Table;

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
int free_memory_index(int pid) {
    for (int i = 0; i < FREE_MEMORY_SIZE; i++) {
        if (available_memory[i] == 0) {
            available_memory[i] = 1;
            page_table_base_register[pid] = i * PAGE_SIZE;
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

// Maps the virtual memory to the physical memory
void map(int pid, int virtual_address, unsigned char value){
    int memory_index;
    if (page_table_base_register[pid] == -1) {  //it means that process isn't mapped yet
        memory_index = free_memory_index(pid); //get the index of first free memory
        if (memory_index == -1) {
            printf("No memory is available");
            return;
        }
        else {
            int virtual_page = get_virtual_page(virtual_address);
            struct Entry Page_table_entry = {value, virtual_page, memory_index, 1};
        }
    }
    else {
        printf("Memory already mapped");
    }

    return;
    
}

// Instructs the memory manager to write  the supplied value
//into the physical memory location associated with the provided
//virtual address
void store(){

}

// Instructs the  memory manager to return the byte stored at the
//memory location specified by virtual_address
void load(){

}

void process_command(int pid, char *command, int vir_addrs, unsigned char value){

    if (pid < 0 && pid > 3){
        printf("PID out of range");
    }

    if (strcmp(command, "map") == 0){
        //map(pid, vir_addrs, value);

    }
    else if (strcmp(command, "store") == 0) {
        //store();
    }
    else if (strcmp(command, "load") == 0) {
        //load();
    }
    else {
        printf("Command not supported");
    }



}

void loop_repl(int argc, char* argv[]){

    unsigned char value;
    int pid, vir_addrs;
    char command[7], exit_check[4];
    
    while(1){

        printf("Instruction? ");
        int arg_count = scanf("%i,%6[^,^\n],%i,%c", &pid, command, &vir_addrs, &value);
        if (arg_count != 4){
            printf("Wrong number of inputs");
        }
        printf("count: %d\n",  arg_count);
        printf("Are you done? (type yes or no) \n");
        scanf("%s", exit_check);

        if (strcmp(exit_check, "yes") == 0){
            exit(1);
        }
        process_command(pid, command, vir_addrs, value);
    }
    
}


int main(int argc, char* argv[]){

    loop_repl(argc, argv);
    
    return 0;
}