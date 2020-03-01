#include "people_threads.h"
#include "data.h"
#include "main.h"
#include "shallow_queue.h"
#include "probability.h"
#include <assert.h>
#include <unistd.h>


/**
 *
 * @param person A pointer to the person struct this thread is representing
 */
void *person_thread(void *person_arg) {
    person *p = (struct person *) person_arg;
    assert(p != NULL);
    assert(p->visits_queue != NULL);

    int arrival_time = add_variance(p->is_pirate ? AVG_PIRATE_ARRIVAL_TIME : AVG_NINJA_ARRIVAL_TIME);
    sleep(arrival_time);

    pthread_mutex_lock(&state_mutex); // == Entering Critical
    p->arrival_time = arrival_time;

    shallow_queue *waiting_line = p->is_pirate ? pirate_queue : ninja_queue;
    enQueue(waiting_line, p);

    pthread_mutex_unlock(&state_mutex); // == Exiting Critical

    sem_post(people_in_line_semaphore);

    pthread_mutex_lock(&p->is_in_fitting_room); // Wait until thread is in fitting room
    pthread_mutex_lock(&state_mutex); // == Re-entering Critical
    // TODO Do math on waiting time
    int shop_time = add_variance(p->is_pirate ? AVG_PIRATE_COSTUME_TIME : AVG_NINJA_COSTUME_TIME);
    struct visit v;
    v.shop_time = shop_time;
    v.wait_time = arrival_time - 99999; // TODO subtract dequeue time
    assert(p->visits_queue != NULL);
    enqueue(p->visits_queue, &v);
    pthread_mutex_unlock(&state_mutex); // == Re-entering Critical
    // TODO print entrance, calculate fitting time, sleep for duration,  etc.
    sleep(1);
    pthread_mutex_unlock(&p->is_in_fitting_room);

    pthread_exit(NULL);
}
