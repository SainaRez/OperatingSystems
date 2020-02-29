#include "people_threads.h"
#include "data.h"
#include "main.h"
#include "shallow_queue.h"
#include <unistd.h>


/**
 *
 * @param person A pointer to the person struct this thread is representing
 */
void *person_thread(void *person_arg) {
    person *p = (struct person *) person_arg;


    printf("Here: \n");
    sleep(2); // TODO sleep random time based on AVG_...

    printf("Here, done sleeping\n"); // TODO temp

    pthread_mutex_lock(&state_mutex); // == Entering Critical

    shallow_queue *waiting_line = p->is_pirate ? pirate_queue : ninja_queue;
    enQueue(waiting_line, p);
    int sem_value;
    sem_getvalue(people_in_line_semaphore, &sem_value);
    printf("SemValue: %i\n", sem_value); // TODO temp

    pthread_mutex_unlock(&state_mutex); // == Exiting Critical

    sem_post(people_in_line_semaphore);

    pthread_mutex_lock(&p->is_in_fitting_room); // Wait until thread is in fitting room
    // TODO print entrance, calculate fitting time, sleep for duration,  etc.
    sleep(1);
    pthread_mutex_unlock(&p->is_in_fitting_room);

    pthread_exit(NULL);
}
