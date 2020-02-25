#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>


#define MIN_THREAD_NUM 50
#define MAX_THREAD_NUM 10

#define MIN_TEAM 2
#define MAX_TEAM 4

#define ARGUMENT_NUMBER 7
 
/**
 * 
 */
typedef struct Person {
    char flag;
    int id;
    pthread_t thread;
    bool coming_back;
    int waiting_time_before_visit;
    double arrival_time;
    int fitting_time;

} Person;

Person pirate_list[51];
Person ninja_list[51];


/**
 * Implementation of Queue
 * This code is taking from the internet
 * source: 
 */

// A linked list (LL) node to store a queue entry 
typedef struct QNode { 
    Person current_person; 
    struct QNode* next; 
} QNode;
  
// The queue, front stores the front node of LL and rear stores the 
// last node of LL 
typedef struct Queue { 
    struct QNode *front, *rear; 
} Queue; 
  
// A utility function to create a new linked list node. 
QNode* newNode(Person p) 
{ 
    //struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode)); 
    QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->current_person = p; 
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
void enQueue(Queue* q, Person p) 
{ 
    // Create a new LL node 
    //struct QNode* temp = newNode(k); 
      QNode* temp = newNode(p); 


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
Person* deQueue(Queue* q) 
{ 
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
        return NULL; 
  
    // Store previous front and move front one node ahead 
    QNode* temp = q->front; 
    Person node_struct = temp->current_person;
  
    q->front = q->front->next; 
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
        q->rear = NULL; 
  
    return &node_struct;
    //free(temp); 
} 

Queue* pirate_queue;
Queue* ninja_queue;

// End of Queue Section

void sort_list(Person person_list[]) {
    int i, j, n;
    Person a;
    // printf("Enter the value of N \n");
    // scanf("%d", &n);

    // printf("Enter the numbers \n");
    // for (i = 0; i < n; ++i)
    //     scanf("%d", &number[i]);

    for (i = 0; i < n; ++i) {
        for (j = i + 1; j < n; ++j) {
            if (person_list[i].arrival_time > person_list[j].arrival_time) {
                a = person_list[i];
                person_list[i] = person_list[j];
                person_list[j] = a;
            }
        }
    }

    printf("The numbers arranged in ascending order are given below \n");
    for (i = 0; i < n; ++i)
        printf("%fl\n", person_list[i].arrival_time);
}

int add_variance(int avg_time) {
    
    srand(time(0));
    int seed = rand();
    srand48(seed);
    double a = drand48();
    double b = drand48();
    double variance = sqrt(-2 * log(a)) * cos(2 * M_PI * b);
    if (variance < 0 && (fabs(variance) >= avg_time)) {
        variance = fabs(variance);
    }
    double updated_time = avg_time + variance;
    return updated_time;
}

void *put_on_costume(int dressing_time) {
    sleep(dressing_time);
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

void create_new_person(int id, int avg_costume_time, int avg_arrival_time, 
                    Person new_person) {
    new_person.id = id;
    new_person.coming_back = is_coming_back();
    new_person.fitting_time = add_variance(avg_costume_time);
    new_person.arrival_time = add_variance(avg_arrival_time);
    pthread_create(&new_person.thread, NULL, put_on_costume(avg_costume_time), NULL);
    //enQueue(q, new_person);
    return;
}
 

void initialize_pirate_threads(int num_pirates, int avg_pirate_costume_time, 
                               int avg_pirate_arrival_time, int num_teams) {

    for (int i = 0; i < (num_pirates);  i++) {
        Person new_pirate;
        new_pirate.flag = 'p';
        create_new_person(i, avg_pirate_costume_time, avg_pirate_arrival_time, new_pirate);
        pirate_list[i] = new_pirate;
    }
    return;
}

void initialize_ninja_threads(int num_ninjas, int avg_ninja_costume_time, 
                            int avg_ninja_arrival_time, int num_teams) {

    for (int i = 51; i < (num_ninjas + 51); i++) {
        Person new_ninja;
        new_ninja.flag = 'n';
        create_new_person(i, avg_ninja_costume_time, avg_ninja_arrival_time, new_ninja);
        ninja_list[i - 51] = new_ninja;
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
    initialize_pirate_threads(num_pirates, avg_pirate_costume_time, avg_pirate_arrival_time, num_teams);
    initialize_ninja_threads(num_ninjas, avg_ninja_costume_time, avg_ninja_arrival_time, num_teams);
    //avg_pirate_costume_time = add_variance(avg_pirate_costume_time);
    //avg_ninja_costume_time = add_variance(avg_ninja_costume_time);
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
