#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "thread_demo.h"
#include "queue.h"
#include "probability.h"
#include "data.h"
#include "shallow_queue.c"
#include "people_threads.h"

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
extern pthread_mutex_t state_mutex;

// The following global variables encapsulate "state" as referenced above
shallow_queue *pirate_queue;
shallow_queue *ninja_queue;
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
    return false;
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
    return NULL; // TODO
}

/**
 * Creates structs for all pirates and ninjas stored on the heap.
 *
 * Access to the initialized people can be done via global_pirate_list and global_ninja_list
 * @param num_pirates
 * @param num_ninjas
 */
void initialize_people(int num_pirates, int num_ninjas) {
    for (int i = 0; i < num_pirates; ++i) {
        create_new_person(true, i);
    }
    for (int i = 0; i < num_ninjas; ++i) {
        create_new_person(false, i);
    }
}

pthread_t global_thread_id_array[100];

/**
 * Runs a person_thread for a given person struct.
 *
 * Persons are initialized via initialize_people(), and this method is designed for use with process_queue();
 *
 * @see initialize_people()
 * @see process_queue()
 * @see person_thread()
 * @arg p Pointer to person struct in memory
 */
void *start_person_thread(person *p) {
    // The static variable below acts as a global variable that increments each function call
    static int global_thread_count = 0;
    pthread_create(&global_thread_id_array[++global_thread_count], NULL, person_thread, p);
    return NULL;
}

/**
 * This function takes in the command line arguments and sets them to descriptive variables,
 * performs error checking for the given arguments and initializes the pirates and the ninjas
 * @param argc the number of command line arguments, argv the arguments themselves
 */
void process_input(int argc, int arguments[]) {
    NUM_TEAMS = arguments[0];
    const int num_pirates = arguments[1];
    const int num_ninjas = arguments[2];
    AVG_PIRATE_COSTUME_TIME = arguments[3];
    AVG_NINJA_COSTUME_TIME = arguments[4];
    AVG_PIRATE_ARRIVAL_TIME = arguments[5];
    AVG_NINJA_ARRIVAL_TIME = arguments[6];

    if (NUM_TEAMS < 2 || NUM_TEAMS > 4) {
        fprintf(stderr, "Illegal argument %i given for number of teams\n", NUM_TEAMS);
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


    // Set up global state variables
    initialize_people(num_pirates, num_ninjas);
    // Queue *pirate_queue = createQueue();
    // Queue *ninja_queue = createQueue();

    printf("-- Starting Simulation --\n");
    // Start a thread for each person
    process_queue(global_pirate_list, start_person_thread);
    process_queue(global_ninja_list, start_person_thread);

    /*
     * TODO, main thread stuff
    */

    // Join back all threads
    for (int i = 0; i < num_ninjas + num_pirates; ++i) {
        pthread_join(global_thread_id_array[i], NULL);
    }

    print_pirates();
    // TODO print pirate summary : All of the Pirates cost 51 gold pieces
    print_ninjas();
    // All of the Ninjas cost 41 gold pieces

    // TODO print summaries:
    /*
     * Expenses for employing the costume teams is: 10 gold pieces
     * Team 0 was busy for 44 minutes and free for 37 minutes.
     * Team 1 was busy for 59 minutes and free for 22 minutes.
     * Average queue length was 5.148148 people
     * Gross revenue is 92 gold pieces
     * Gold per visit: 3.538462
     * Total revenue: 82
     */

    // Cleanup
    destroy_queue(global_pirate_list);
    destroy_queue(global_ninja_list);
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
