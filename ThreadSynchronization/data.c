#include "data.h"
#include <stdio.h>
#include <pthread.h>
#include "queue.h"

bool are_global_lists_initialized = false;
/** Global list of all pirates and their structs */
queue *global_pirate_list = NULL;
/** Global list of all ninjas and their structs */
queue *global_ninja_list = NULL;
//TODO


void initialize_global_lists() {
    global_pirate_list = create_queue(sizeof(struct person));
    global_ninja_list = create_queue(sizeof(struct person));
    are_global_lists_initialized = true;
}

/**
 * Initializes a new person at given memory address
 * @param is_pirate
 * @param id
 * @param person_pointer Memory address (in heap) where person will be stored
 */
void create_new_person(bool is_pirate, int id) {
    if (are_global_lists_initialized == false) {
        initialize_global_lists();
    }

    struct person p;
    p.is_pirate= is_pirate;
    p.id = id;
    p.is_pirate ? enqueue(global_pirate_list, &p) : enqueue(global_ninja_list, &p);

    pthread_mutex_init(&p.is_in_fitting_room, NULL);
    pthread_mutex_lock(&p.is_in_fitting_room);
}

#define TITLE p->is_pirate ? "Pirate" : "Ninja"

void print_person(struct person *p) {
    printf("%s %i:\n", TITLE, p->id);
    // TODO print visits
    printf("Total number of visits %i. ", 55555); // TODO visits
    printf("Cost for %s %i: %i\n", TITLE, p->id, 55555); // TODO cost
}


/**
 * Calculates the cost of the visit based.
 * @param v Pointer to visit structure
 * @return Cost in gold of the visit baesd on wait time and shop time.
 */
int get_cost_of_visit(struct visit *v) {
    return -1;
    // TODO
}


void print_visit(struct visit *v) {
    // TODO
}

/**
 * Prints all pirates in order according to the assignment template.
 *
 * Output example:
 *
 * Pirate 1:
 * Visit 3: Waited for 0 minutes and was in shop for 5 minutes.
 * Visit 2: Waited for 4 minutes and was in shop for 4 minutes.
 * Visit 1: Waited for 45 minutes and was in shop for 6 minutes.
 * Total number of visits: 3. Cost for Pirate 1: 9
 */
void print_pirates() {
    // Causes warning, but warning is fine as casting would just be more confusing
    process_queue(global_pirate_list, print_person);
}

void print_ninjas() {
    process_queue(global_ninja_list, print_person);
}
