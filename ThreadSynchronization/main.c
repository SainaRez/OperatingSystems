#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>


#define MIN_THREAD_NUM 50
#define MAX_THREAD_NUM 10

#define MIN_TEAM 2
#define MAX_TEAM 4

#define ARGUMENT_NUMBER 7

typedef struct Ninja {

    bool coming_back;
    int waiting_time_before_visit;

} Ninja;

typedef struct Pirate {
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
    int key; 
    struct QNode* next; 
} QNode;
  
// The queue, front stores the front node of LL and rear stores the 
// last node of LL 
typedef struct Queue { 
    struct QNode *front, *rear; 
} Queue; 
  
// A utility function to create a new linked list node. 
QNode* newNode(int k) 
{ 
    //struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode)); 
    QNode* temp;
    temp->key = k; 
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
void enQueue(Queue* q, int k) 
{ 
    // Create a new LL node 
    //struct QNode* temp = newNode(k); 
      QNode* temp = newNode(k); 


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
    int node_value = temp->key;
  
    q->front = q->front->next; 
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
        q->rear = NULL; 
  
    return temp->key;
    //free(temp); 
} 

Queue* pirate_queue;
Queue* ninja_queue;

// End of Queue Section



// void add_variance_to_inputs(int argc, int arguments[]) {
    
//     srand(time(0));
//     int seed = rand();
//     srand48(seed);
//     double variance;
//     variance = drand48();
//     printf("First variance: %f\n",  variance);
//     (double)arguments[3] += variance;
//     variance = drand48();
//     printf("Second variance: %f\n",  variance);
//     (double)arguments[4] += variance;

//     //This needs to be somewhere else but I'll move it when the function is written
    
//     double random_number = rand() % 4;
//     if (random_number == 2) {
//         //Person is coming back
//     } 
    
    
//     return;
// }


void process_input(int argc, int arguments[]) {
    if (argc != ARGUMENT_NUMBER) {
        printf("Incorrect number of arguments\n");
        return;
    }
    if (arguments[0] < 2 || arguments[0] > 4) {
        printf("Incorrect number of teams\n");
        return;
    }
    if (arguments[1] < 10 || arguments[1] > 50) {
        printf("Number of threads (pirates) is out of range\n");
        return;
    }
    if (arguments[2] < 10 || arguments[2] > 50) {
        printf("Number of threads (ninjas) is out of range\n");
        return;
    }
    
    return;
}


int main(int argc, char *argv[]) {
    
    int arguments[ARGUMENT_NUMBER];
   
    for (int i = 1; i < argc; i++) {
        arguments[i] = atoi(argv[i]);
    }
    arguments[argc] = '\0';

    process_input(argc, arguments);

    return 0;
}