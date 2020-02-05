#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> // should always include errno when doing system calls to debug
#include <stdlib.h> // included for exit()
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>


// Maps  the virtual memory tothe physical memory
void map(){

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


int main(int argc, char* argv[]){

    int pid, vir_addrs, value;
    char command[512], exit_check[512];
    
    while(1){

        printf("Instruction?  \n");
        scanf("%i,%s,%i,%i\n", &pid, command, &vir_addrs, &value);

        printf("Are you done? (type yes or no) \n");
        scanf("%s", exit_check);

        if (strcmp(exit_check, "yes") == 0){
            exit(1);
        }
    }
    
    return 0;
}