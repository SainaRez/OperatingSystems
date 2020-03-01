#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>
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
pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;

// The following global variables encapsulate "state" as referenced above
shallow_queue *pirate_queue = NULL;
shallow_queue *ninja_queue = NULL;
/** False if the fitting room is empty or has ninjas */
bool does_fitting_room_have_pirates;
/** False if the fitting room is empty or has pirates */
bool does_fitting_room_have_ninjas;
/** dressing_room_is_empty[n] is true if room n is empty*/
bool dressing_room_is_empty[MAX_TEAM];

sem_t *people_in_line_semaphore = NULL;
sem_t *teams_free_semaphore = NULL;


/**
 * Frees up the given dressing room team and updates associated semaphore for teams.
 * Requires state_mutex to be free on call.
 * @param team_num Team that is to be marked as now free
 */
void free_dressing_room_team(int team_num) {
    assert(dressing_room_is_empty[team_num] == false); // Shouldn't be freeing an already free team
    pthread_mutex_lock(&state_mutex);
    int i;
    sem_getvalue(teams_free_semaphore, &i);
    if (i == 1) {
        // We are completely freeing up the dressing room by freeing this
        does_fitting_room_have_ninjas = false;
        does_fitting_room_have_pirates = false;
    }
    dressing_room_is_empty[team_num] = true;
    sem_post(teams_free_semaphore);
    pthread_mutex_unlock(&state_mutex);
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

pthread_t global_thread_id_array[100]; // TODO Should this be cleaned up?

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
 * Returns the arrival time of the person who is next to enter the store.
 * (The person with the lower arrival time has been in line longest).
 *
 * @param is_pirate_wait_time
 * @return INT_MAX if no one is in line
 */
int get_next_persons_arrival_time(bool is_pirate_wait_time) {
    if (is_pirate_wait_time) {
        if (pirate_queue->front == NULL) return INT_MAX;
        return pirate_queue->front->current_person->arrival_time;
    } else {
        if (ninja_queue->front == NULL) return INT_MAX;
        return ninja_queue->front->current_person->arrival_time;
    }
}

/**
 * Returns the next available team number
 * @return Team number that you should put the next person into
 */
int next_available_team() {
    for (int i = 0; i < NUM_TEAMS; ++i) {
        if (dressing_room_is_empty[i] == true) {
            return i;
        }
    }

    fprintf(stderr, "Illegal State exception- unreachable code in next_available_team()\n");
    assert(false);
}

/**
 * Called from run_store
 * @param is_person_a_pirate
 */
void dequeue_next_person_to_store(bool is_person_a_pirate) {
    struct person *person_to_enter_store;

    if (is_person_a_pirate) {
        person_to_enter_store = deQueue(pirate_queue);
        printf("Dequeued pirate %i\n", person_to_enter_store->id);
        assert(does_fitting_room_have_ninjas == false);
        does_fitting_room_have_pirates = true;
    } else {
        person_to_enter_store = deQueue(ninja_queue);
        printf("Dequeued ninja %i\n", person_to_enter_store->id);
        assert(does_fitting_room_have_pirates == false);
        does_fitting_room_have_pirates = true;
    }


    // TODO put them in the next available store slot
    int team_num = next_available_team();
    person_to_enter_store->assigned_team = team_num;
    dressing_room_is_empty[team_num] = false;

    int i;
    sem_getvalue(teams_free_semaphore, &i);
    printf("Teams_free_semaphore: %i\n", i);
    // TODO is this okay? (It doesn't appear so)
    sem_wait(teams_free_semaphore); // decrement the number of free teams

    pthread_mutex_unlock(&person_to_enter_store->is_in_fitting_room);
}

/**
 * Test function wrapper for add_variance
 * @param any average value
 */
void variance_test(int avg_time) {
    int i = 0;
    srand(time(NULL));
    rand();
    while (i < 50) {
        add_variance(avg_time);
        i++;
    }
    return;
}

/**
 * This executes the main thread loop of managing the store.
 *
 * This should be called once all setup is complete and all Person threads are running
 */
void run_store() {
    while (true) { // TODO keep track of how many Persons are still active
        sem_wait(people_in_line_semaphore);
        // At least one person is in line, now wait for a free room/team
        printf(" ---- Waiting for free team\n");
        sem_wait(teams_free_semaphore);
        // A person is in line, and there is a free room.
        pthread_mutex_lock(&state_mutex);
        // TODO strategy, how to avoid letting pirates hog the room?
        int pirate_arrival_time = get_next_persons_arrival_time(true);
        int ninja_arrival_time = get_next_persons_arrival_time(false);
        int wait_time_difference =
                pirate_arrival_time - ninja_arrival_time; // Useful for when there is a large discrepancy
        bool have_pirates_waited_longer = (wait_time_difference < 0);

        if (does_fitting_room_have_pirates) {
            if (have_pirates_waited_longer) {
                // Trivial decision
                dequeue_next_person_to_store(true);
            } else {
                if (pirate_arrival_time == INT_MAX) {
                    // Pirate line is empty
                    exit(EXIT_FAILURE);
                    // TODO Just wait more somehow ...
                } else {
                    // This is where a decision needs to be made based on wait_time_difference
                    dequeue_next_person_to_store(true); // TODO temp, for now, just let pirates block
                }
            }
        } else if (does_fitting_room_have_ninjas) {
            if (!have_pirates_waited_longer) {
                // Trivial decision
                dequeue_next_person_to_store(false);
            } else {
                if (ninja_arrival_time == INT_MAX) {
                    // No ninjas in line
                    exit(EXIT_FAILURE);
                    // TODO Just wait more somehow ...
                } else {
                    // This is where a decision needs to be made based on wait_time_difference
                    dequeue_next_person_to_store(false); // TODO temp for now just let ninjas block
                }
            }
        } else {
            if (have_pirates_waited_longer) {
                dequeue_next_person_to_store(true);
            } else {
                dequeue_next_person_to_store(false);
            }
        }


        pthread_mutex_unlock(&state_mutex);
    }

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
    variance_test(AVG_NINJA_ARRIVAL_TIME);


    // Set up global state variables
    initialize_people(num_pirates, num_ninjas);
    pirate_queue = createQueue();
    ninja_queue = createQueue();

    for (int i = 0; i < NUM_TEAMS; ++i) {
        dressing_room_is_empty[i] = true;
    }

    people_in_line_semaphore = malloc(sizeof(sem_t));
    sem_init(people_in_line_semaphore, 1, 0); // Semaphore is initially locked, as no one is in line
    teams_free_semaphore = malloc(sizeof(sem_t));
    sem_init(teams_free_semaphore, 1, NUM_TEAMS); // Semaphore is initially not locked, as all teams are free

    printf("-- Starting Simulation --\n");
    // Start a thread for each person
    process_queue(global_pirate_list, start_person_thread);
    process_queue(global_ninja_list, start_person_thread);

    run_store();

    // Join back all threads
    for (int i = 0; i < num_ninjas + num_pirates; ++i) {
        pthread_join(global_thread_id_array[i], NULL);
    }

    // process_shallow_queue(ninja_queue, print_person); // Useful way to check state of lines

    print_pirates();
    // TODO print pirate summary : All of the Pirates cost 51 gold pieces
    print_ninjas();
    // TODO All of the Ninjas cost 41 gold pieces

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
