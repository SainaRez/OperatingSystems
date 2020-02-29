#pragma once

#include <pthread.h>
#include <stdbool.h>

struct visit {
    unsigned int wait_time;
    unsigned int shop_time;
};

typedef struct person {
    int id;
    bool is_pirate;
    /** Unlocked when Person is in a fitting room */
    pthread_mutex_t is_in_fitting_room;
    /** Value is only relevant when Person is in a fitting room */
    int assigned_team;
} person;

void create_new_person(bool is_pirate, int id, person* person_pointer);

void print_person(struct person *p);

int get_cost_of_visit(struct visit *v);

void print_visit(struct visit *v);



