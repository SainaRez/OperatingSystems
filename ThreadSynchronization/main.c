#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>


#define MIN_THREAD_NUM 50
#define MAX_THREAD_NUM 10

#define MIN_TEAM 2
#define MAX_TEAM 4

#define ARGUMENT_NUMBER 7
 

typedef struct Ninja {
    int id;
    pthread_t thread;
    bool coming_back;
    int waiting_time_before_visit;

} Ninja;

typedef struct Pirate {
    int id;
    pthread_t thread;
    bool coming_back;
    int waiting_time_before_visit;

} Pirate;

/**
 * Implementation of Queue
 * This code is taking from the internet
 * source: 
 */

// A linked list (LL) node to store a queue entry 
typedef struct QNode { 
    pthread_t current_thread; 
    struct QNode* next; 
} QNode;
  
// The queue, front stores the front node of LL and rear stores the 
// last node of LL 
typedef struct Queue { 
    struct QNode *front, *rear; 
} Queue; 
  
// A utility function to create a new linked list node. 
QNode* newNode(pthread_t t) 
{ 
    //struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode)); 
    QNode* temp;
    temp->current_thread = t; 
    temp->next = NULL; 
    return temp; 
};
  
// A utility function to create an empty queue 
Queue* createQueue() 
{ 
    Queue* q = (Queue*)malloc(sizeof(Queue)); 
    //Queue* q;
    q->front = q->rear = NULL; 
    return q; 
}
  
// The function to add a key k to q 
void enQueue(Queue* q, pthread_t t) 
{ 
    // Create a new LL node 
    //struct QNode* temp = newNode(k); 
      QNode* temp = newNode(t); 


    // If queue is empty, then new node is front and rear both 
    if (q->rear == NULL) { 
        q->front = q->rear = temp; 
        return; 
    } 
  
    // Add the new node at the end of queue and change rear 
    q->rear->next = temp; 
    q->rear = temp; 
} 
  
// Function to remove a key from given queue q 
int deQueue(Queue* q) 
{ 
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
        return -1; 
  
    // Store previous front and move front one node ahead 
    QNode* temp = q->front; 
    int node_thread = temp->current_thread;
  
    q->front = q->front->next; 
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
        q->rear = NULL; 
  
    return node_thread;
    //free(temp); 
} 

Queue* pirate_queue;
Queue* ninja_queue;

// End of Queue Section



int add_variance(int preparation_time) {
    
    srand(time(0));
    int seed = rand();
    srand48(seed);
    double variance;
    variance = drand48();
    
    double updated_time = preparation_time + variance;
    return updated_time;
}

void *fill_in_function() {
    sleep(1);
    return NULL;
}

bool is_coming_back() {
    srand(time(0));
    double random_number = rand() % 4;
    if (random_number == 0) {
        return true;
    }
    return false;
}
 

void add_pirate_to_queue(int num_pirates) {

    for (int i = 0; i < (num_pirates);  i++) {
        Pirate *new_pirate;
        new_pirate->id = i;
        new_pirate->coming_back = is_coming_back();  
        pthread_create(&new_pirate->thread, NULL, fill_in_function, NULL);
        enQueue(pirate_queue, new_pirate);
    }
    return;
}

void add_ninja_to_queue(int num_ninjas) {

    for (int i = 51; i < (num_ninjas + 51); i++) {
        Ninja *new_ninja;
        new_ninja->id = i;
        new_ninja->coming_back = is_coming_back();
        pthread_create(&new_ninja->thread, NULL, fill_in_function, NULL);
        enQueue(ninja_queue, new_ninja);
    }
    return;
}


void process_input(int argc, int arguments[]) {
    int num_teams = arguments[0];
    int num_pirates = arguments[1];
    int num_ninjas = arguments[2];
    int avg_pirate_costume_time = arguments[3];
    int avg_ninja_costume_time = arguments[4];
    int avg_pirate_arrival_time = arguments[5];
    int avg_ninja_arrival_time = arguments[6];

    if (num_teams < 2 || num_teams > 4) {
        fprintf(stderr, "Illegal argument %i given for number of teams\n", num_teams);
        exit(EXIT_FAILURE);
    }
    if (num_pirates < 10 || num_pirates > 50) {
        fprintf(stderr, "Number of threads (pirates) %i is out of range\n", num_pirates);
        exit(EXIT_FAILURE);
    }
    if (num_ninjas < 10 || num_ninjas > 50) {
        fprintf(stderr,"Number of threads (ninjas) %i is out of range\n", num_ninjas);
        exit(EXIT_FAILURE);
    }
    avg_pirate_costume_time = add_variance(avg_pirate_costume_time);
    avg_ninja_costume_time = add_variance(avg_ninja_costume_time);
    return;
}


int main(int argc, char *argv[]) {
    if (argc != ARGUMENT_NUMBER + 1) {
        fprintf(stderr, "Incorrect number of arguments %i given\n", argc);
        exit(EXIT_FAILURE);
    }

    int arguments[ARGUMENT_NUMBER];

    for (int i = 0; i < ARGUMENT_NUMBER; i++) {
        // TODO validate that input is actually a number
        arguments[i] = atoi(argv[i + 1]);
    }

    process_input(argc, arguments);
    printf("-- Starting Simulation --\n");

    return 0;
}
