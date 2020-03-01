#pragma once

#include <pthread.h>
#include <stdbool.h>
#include "queue.h"

struct visit {
    int wait_time;
    int shop_time;
} visit;

typedef struct person {
    int id;
    bool is_pirate;
    /** Unlocked when Person is in a fitting room */
    pthread_mutex_t is_in_fitting_room;
    /** Value is only relevant when Person is in a fitting room */
    int assigned_team;
    /** Value is only relevant when the Person in currently in line*/
    int arrival_time;
    /** A list of visits to the store */
    queue *visits_queue;
} person;

extern queue *global_pirate_list;
extern queue *global_ninja_list;

void create_new_person(bool is_pirate, int id);

void print_person(struct person *p);

int get_cost_of_visit(struct visit *v);

void print_visit(struct visit *v);

void print_pirates();

void print_ninjas();

