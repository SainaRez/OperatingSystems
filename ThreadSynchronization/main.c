#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
// #include <unistd.h>
#include <pthread.h>
#include "thread_demo.h"
#include "queue.h"
#include "probability.h"
#include "data.h"

#define MIN_THREAD_NUM 50
#define MAX_THREAD_NUM 10

#define MIN_TEAM 2
#define MAX_TEAM 4


/**
 * These variables are constant between all threads, and should never
 * be modified outside of their initialization early in main();
 */
int AVG_PIRATE_COSTUME_TIME;
int AVG_NINJA_COSTUME_TIME;
int AVG_PIRATE_ARRIVAL_TIME;
int AVG_NINJA_ARRIVAL_TIME;
int NUM_TEAMS;


/** The following mutex should be locked any time program state is being used */
pthread_mutex_t state_mutex;

// The following global variables encapsulate "state" as referenced above
queue *pirate_queue;
queue *ninja_queue;
/** False if the fitting room is empty or has ninjas */
bool does_fitting_room_have_pirates;
/** False if the fitting room is empty or has pirates */
bool does_fitting_room_have_ninjas;


/**
 * Checks if both queues are empty.
 * @return True if both queues to the shop are empty.
 */
bool are_both_queues_empty() {
    // TODO
    // return (pirate_queue->front == NULL && ninja_queue->front == NULL);
}

/**
 * Compares the waiting_time_before_visit of the Pirate and Ninja who have been in line the longest,
 * (i.e., which Person at the front of their line has been waiting in line to get in the store the longest).
 *
 * Exits/Fails if both lines are empty when function is called
 *
 * @return True if the pirates are next in line.
 */
bool do_pirates_have_next_in_line() {
    assert(are_both_queues_empty() == false);
    // TODO
    //return (pirate_queue->front->current_person.waiting_time_before_visit >=
    //       ninja_queue->front->current_person.waiting_time_before_visit);
    return false;
}


void fitting_room() {

}


void *enter_queue(person p) {
}

void initialize_people(int num_pirates, int num_ninjas) {
    for (int i = 0; i < num_pirates; ++i) {
        create_new_person(true, i + 1); // i + 1 as id's start at 1
    }
    for (int i = 0; i < num_ninjas; ++i) {
        create_new_person(false, i + 1);
    }

    print_pirates();
}

/**
 * Creates a Person struct for each of the ninjas, sets their flag to be
 * identified as a ninja and adds them to a ninja list
 *
 * @param num_ninjas
 */
void initialize_threads(int num_ninjas, int num_pirates, int num_teams) {
    // TODO maybe do this in data from global queue thingy?
    for (int i = 0; i < num_ninjas; ++i) {

    }

    for (int i = 0; i < num_pirates; ++i) {
        // TODO create pirates
    }
}

/**
 * This function takes in the command line arguments and sets them to descriptive variables,
 * performs error checking for the given arguments and initializes the pirates and the ninjas
 * @param argc the number of command line arguments, argv the arguments themselves
 */
void process_input(int argc, int arguments[]) {
    int num_teams = arguments[0];
    int num_pirates = arguments[1];
    int num_ninjas = arguments[2];
    AVG_PIRATE_COSTUME_TIME = arguments[3];
    AVG_NINJA_COSTUME_TIME = arguments[4];
    AVG_PIRATE_ARRIVAL_TIME = arguments[5];
    AVG_NINJA_ARRIVAL_TIME = arguments[6];

    if (num_teams < 2 || num_teams > 4) {
        fprintf(stderr, "Illegal argument %i given for number of teams\n", num_teams);
        exit(EXIT_FAILURE);
    }
    if (num_pirates < 10 || num_pirates > 50) {
        fprintf(stderr, "Number of threads (pirates) %i is out of range\n", num_pirates);
        exit(EXIT_FAILURE);
    }
    if (num_ninjas < 10 || num_ninjas > 50) {
        fprintf(stderr, "Number of threads (ninjas) %i is out of range\n", num_ninjas);
        exit(EXIT_FAILURE);
    }


    initialize_people(num_pirates, num_ninjas);
    // Queue *pirate_queue = createQueue();
    // Queue *ninja_queue = createQueue();
    printf("-- Starting Simulation --\n");

    // TODO pthread joins
}


int main(int argc, char *argv[]) {
    // run_queue_test();
    // run_thread_demo();

    const int ARGUMENT_NUMBER = 7;
    if (argc != ARGUMENT_NUMBER + 1) {
        fprintf(stderr, "Incorrect number of arguments %i given\n", argc);
        exit(EXIT_FAILURE);
    }

    int arguments[ARGUMENT_NUMBER];

    for (int i = 0; i < ARGUMENT_NUMBER; i++) {
        arguments[i] = atoi(argv[i + 1]);
    }

    process_input(argc, arguments);

    exit(EXIT_SUCCESS);
}
