#include "data.h"
#include <stdio.h>
#include "queue.h"

bool are_global_lists_initialized = false;
/** Global list of all pirates and their structs */
queue *global_pirate_list;
/** Global list of all ninjas and their structs */
queue *global_ninja_list;


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

    person *p = (person *) malloc(sizeof(struct person));
    p->is_pirate = is_pirate;
    p->id = id;
    enqueue(global_pirate_list, p);
    // p->is_pirate ? enqueue(global_pirate_list, p) : enqueue(global_ninja_list, p);
    // TODO the rest of the stuff
    print_person(p);
}

void print_person(struct person *p) {
    // TODO
    printf("Person ID: %i, is_pirate: %i\n", p->id, p->is_pirate);
}


/**
 * Calculates the cost of the visit based.
 * @param v Pointer to visit structure
 * @return Cost in gold of the visit baesd on wait time and shop time.
 */
int get_cost_of_visit(struct visit *v) {
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
