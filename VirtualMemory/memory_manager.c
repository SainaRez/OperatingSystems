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
#define FRAME_SIZE 16

unsigned char memory[MAX_MEMORY_SIZE];

int page_table_base_register[PTBR_SIZE];

int available_memory[FREE_MEMORY_SIZE];

typedef struct Entry {
    unsigned char protectionBits;
    unsigned char is_used;
    unsigned char virtual_page;
    unsigned char physical_page;
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

//checks if a memory is available and retunr the first index
int free_memory_index(int pid) {
    for (int i = 0; i < FREE_MEMORY_SIZE; i++) {
        if (available_memory[i] == 0) {
            available_memory[i] = 1;
            page_table_base_register[pid] = i * FRAME_SIZE;
            return i;
        }
        else {
            if (i == FREE_MEMORY_SIZE - 1) {
                printf("No frame available");
                return -1;
            }
        }
    }
}

// Maps  the virtual memory tothe physical memory
void map(int pid){
    int memory_index;
    if (page_table_base_register[pid] == -1) {
        memory_index = free_memory_index(pid);
        if (memory_index == -1) {
            printf("no memory is available");
            return;
        }
        else {
            int virtual_page = (virtual_address - (virtual_addres % FRAME_SIZE)) / FRAME_SIZE;
            Entry *page_table_base_register = 
        }
    }





    return;
    // if (page_table_base_register[pid] == -1) {
        
    //     for(int i = 0; i < FREE_MEMORY_SIZE; i++) {
    //         if (available_memory[i] == 0) {
    //             available_memory[i] = 1; 
    //             page_table_base_register[pid] = i;
    //             exit(1);
    //         }
    //         else {
    //             if(i == FREE_MEMORY_SIZE-1) {
    //                 printf("No frame available");
    //                 return;
    //             }
    //         }
    //     }
        
    //     //int memory_location = (page_table_base_register[pid] * 16) + page;
    // }
    // else {
    //     printf("Page Table is already created for this PID");
    //     return;
    // }
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

void process_command(const int pid, const char *command, const  int vir_addrs, const int value){

    if (pid < 0 && pid > 3){
        printf("PID out of range");
    }

    if (strcmp(command, "map") == 0){
        //map(pid, vir_addrs, value);

    }
    else if (strcmp(command, "store") == 0) {

    }
    else if (strcmp(command, "load") == 0) {

    }
    else {
        printf("Command not supported");
    }



}

void loop_repl(int argc, char* argv[]){

    int value;
    int pid, vir_addrs;
    char command[7], exit_check[4];
    
    while(1){

        printf("Instruction? ");
        int arg_count = scanf("%i,%6[^,^\n],%i,%i", &pid, command, &vir_addrs, &value);
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