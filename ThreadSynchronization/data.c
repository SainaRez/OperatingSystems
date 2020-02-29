#include "data.h"
#include <stdio.h>

/**
 * Initializes a new person at given memory address
 * @param is_pirate
 * @param id
 * @param person_pointer Memory address (in heap) where person will be stored
 */
void create_new_person(bool is_pirate, int id, person *person_pointer) {
    person_pointer->is_pirate = is_pirate;
    person_pointer->id = id;
    // TODO
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
