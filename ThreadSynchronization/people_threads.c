#include "people_threads.h"
#include "data.h"
#include <unistd.h>


/**
 *
 * @param person A pointer to the person struct this thread is representing
 */
void *person_thread(void *person_arg) {
    person *p = (struct person *) person_arg;

    printf("Here: \n");
    sleep(5); // TODO sleep random time based on AVG_...

    printf("Here, done sleeping\n"); // TODO temp
   /*
    pthread_mutex_lock(&state_mutex);
    printf("MUTATING STATE!\n");
    // TODO enter shallow queue
    pthread_mutex_unlock(&state_mutex);
*/
    pthread_exit(NULL);
}
