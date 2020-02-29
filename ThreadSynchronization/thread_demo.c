#include "thread_demo.h"

#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "queue.h"


int sum = 1;
int sum2 = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *pirate(void *arg) {
    double *pointer = (double *) arg;
    double value = *pointer;

    sleep(value);

    if (value / 5 < 1) {
        sleep(3);
    }

    pthread_mutex_lock(&mutex);
    int temp = sum;
    sum += sum2;
    sum2 = temp;
    printf("%f\n", value);
    pthread_mutex_unlock(&mutex);


    pthread_exit(&sum);
}

double get_sleep_time(int x) {
    if (x % 2 == 0) {
        return x + .5;
    }
    return x + .35;
}

/**
 * Runs a test of threads with mutexes
 */
void run_thread_demo() {
    printf("====== Running thread test ======\n");

    pthread_t tids[20];

    for (int i = 0; i < 20; ++i) {
        double *k = malloc(sizeof(double));
        *k = get_sleep_time(i);
        // Create thread
        pthread_create(&tids[i], NULL, pirate, k);
    }

    for (int a = 0; a < 20; ++a) {
        int *ret;
        pthread_join(tids[a], (void *) &ret);
        //printf("%i\n",*ret);
    }
}

/**
 * Used for run_queue_test
 */
struct temp {
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
};

void *print_temp(void *data) {
    struct temp *t = (struct temp *) data;
    printf("%i, %i, %i, %i\n", t->a, t->b, t->c, t->f);
}

/**
 * Runs a test to see that the generic queue is working
 */
void run_queue_test() {
    printf("====== Running queue test ======\n");
    queue *q = create_queue(sizeof(struct temp));
    struct temp t;
    t.a = 10;
    t.b = 10;
    t.c = 10;
    t.d = 10;
    t.e = 10;
    t.f = 11;
    enqueue(q, &t);

    struct temp test;
    peek(q, &test);
    printf("Front of the Q is %d\n", test.a);

    for (int i = 0; i < 10; i++) {
        struct temp a;
        a.c = i + 10;
        a.b = 1;
        a.a = 2;
        a.f = 99;
        enqueue(q, &a);
    }

    process_queue(q, print_temp);

    while (!is_empty(q)) {
        struct temp temp;
        temp = *(struct temp *) dequeue(q);
        printf("Dequeued %d\n", temp.c);
        printf("Size of queue is %i\n", get_length(q));
        process_queue(q, print_temp);
    }
    clear_queue(q);
    printf("Cleared queue\n");
    destroy_queue(q);
    printf("Destroyed queue\n");
}
