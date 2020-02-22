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


void process_input(const int arguments[]) {
    const int num_teams = arguments[0];
    const int num_pirates = arguments[1];
    const int num_ninjas = arguments[2];
    const int avg_pirate_costume_time = arguments[3];
    const int avg_ninja_costume_time = arguments[4];
    const int avg_pirate_arrival_time = arguments[5];
    const int avg_ninja_arrival_time = arguments[6];

    if (num_teams < 2 || num_teams > 4) {
        fprintf(stderr, "Illegal argument %i given for number of teams\n", num_teams);
        exit(EXIT_FAILURE);
    }
    if (num_pirates < 10 || num_pirates > 50) {
        fprintf(stderr, "Number of threads (pirates) %i is out of range\n", num_pirates);
        exit(EXIT_FAILURE);
    }
    if (num_ninjas < 10 || num_ninjas > 50) {
        fprintf(stderr,"Number of threads (ninjas) %i is out of range\n", num_ninjas);
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char *argv[]) {
    if (argc != ARGUMENT_NUMBER + 1) {
        fprintf(stderr, "Incorrect number of arguments %i given\n", argc);
        exit(EXIT_FAILURE);
    }

    int arguments[ARGUMENT_NUMBER];

    for (int i = 0; i < ARGUMENT_NUMBER; i++) {
        // TODO validate that input is actually a number
        arguments[i] = atoi(argv[i + 1]);
    }

    process_input(arguments);
    printf("-- Starting Simulation --\n");
}
