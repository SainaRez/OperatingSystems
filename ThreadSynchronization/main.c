#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define MIN_THREAD_NUM 50
#define MAX_THREAD_NUM 10

#define MIN_TEAM 2
#define MAX_TEAM 4

#define ARGUMENT_NUMBER 7



void process_input(int argc, int arguments[]) {
    if (argc != ARGUMENT_NUMBER) {
        printf("Incorrect number of arguments\n");
        return;
    }
    if (arguments[0] < 2 || arguments[0] > 4) {
        printf("Incorrect number of teams\n");
        return;
    }
    if (arguments[1] < 10 || arguments[1] > 50) {
        printf("Number of threads (pirates) is out of range\n");
        return;
    }
    if (arguments[2] < 10 || arguments[2] > 50) {
        printf("Number of threads (ninjas) is out of range\n");
        return;
    }
    
    return;
}


int main(int argc, char *argv[]) {
    
    int arguments[ARGUMENT_NUMBER];
   
    for (int i = 1; i < argc; i++) {
        arguments[i] = atoi(argv[i]);
    }
    arguments[argc] = '\0';

    process_input(arguments);

    return 0;
}