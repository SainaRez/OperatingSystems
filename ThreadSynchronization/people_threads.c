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

    pthread_mutex_lock(&state_mutex);

    shallow_queue *waiting_line = p->is_pirate ? pirate_queue : ninja_queue;
    enQueue(waiting_line, p);

    pthread_mutex_unlock(&state_mutex);

    // TODO increment semaphore for line
    // TODO wait on p.is_in_fitting_room

    pthread_exit(NULL);
}
