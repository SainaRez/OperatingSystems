#pragma once
#include "shallow_queue.h"
#include <pthread.h>
#include <semaphore.h>

extern shallow_queue *pirate_queue;
extern shallow_queue *ninja_queue;

extern sem_t *people_in_line_semaphore;
extern pthread_mutex_t state_mutex;

extern int AVG_PIRATE_COSTUME_TIME;
extern int AVG_NINJA_COSTUME_TIME;
extern int AVG_PIRATE_ARRIVAL_TIME;
extern int AVG_NINJA_ARRIVAL_TIME;

