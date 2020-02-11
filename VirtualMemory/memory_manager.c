#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> // should always include errno when doing system calls to debug
#include <stdlib.h> // included for exit()
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define PTBR 4
#define MAX_MEMORY 64
#define FREE_MEMORY

unsidgned char memory[MAX_MEMORY];

int page_table_base_register[PTBR];

int available_memory[FREE_MEMORY];

void initialize_arrays(){
    for(int i = 0; i < PTBR; i++){
        page_table_base_register[i] = -1;
    }

    for(int i = 0; i < FREE_MEMORY; i++){
        available_memory[i] = 0;
    }
    return;
}


// Maps  the virtual memory tothe physical memory
void map(int pid){
    if (page_table_base_register[pid] == -1) {
        
        for(int i = 0; i < FREE_MEMORY; i++) {
            if (available_memory[i] == 0) {
                available_memory[i] = 1; 
                page_table_base_register[pid] = i;
                exit(1);
            }
        }
    }

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

void process_command(int pid, char command, int vir_addrs, int value){

    if (pid < 0 && pid > 3){
        printf("PID out of range");
    }

    if (strcmp(command, "map")){
        map(pid, vir_addrs, value);

    }
    else if (strcmp(command, "store")) {

    }
    else if (strcmp(command, "load")) {

    }
    else {
        printf("Command not supported");
    }



}

void loop_repl(int argc, char* argv[]){

    int pid, vir_addrs, value;
    char command[512], exit_check[512];
    
    // while(1){

    //     printf("Instruction?  ");
    //     int arg_count = scanf("%i,%s,%i,%i\n", &pid, command, &vir_addrs, &value);
    //     if (arg_count != 4){
    //         printf("Wrong number of inputs");
    //     }
    //     printf("count: %d\n",  arg_count);
    //     printf("Are you done? (type yes or no) \n");
    //     scanf("%s", exit_check);

    //     if (strcmp(exit_check, "yes") == 0){
    //         exit(1);
    //     }
    // }
    pid = 1;
    vir_addrs = 2;
    //command = "map";
    value = 0;
}


int main(int argc, char* argv[]){

    loop_repl(argc, argv);
    
    return 0;
}