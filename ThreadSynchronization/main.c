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

// Redefining fitting room to have the second argument as default set to zero
#define FITTING_ROOM(a, ...) fitting_room( a, (0, ##__VA_ARGS__))
 
/**
 * Person  struct which is used to initize each pirate and ninja
 * @param flag: determines if struct is a pirate or ninja, id: each Person
 * has a unique id, thread: the thread for that paerson, coming back: if the
 * person is coming back or not, waiting_time_before_visit: how long was
 * person waiting in queue, arrival_time: the arrival time of the person at
 * the queue, fitting_time: The time it takes the person to be dressed
 */
typedef struct Person {
    char flag;
    pthread_t thread;
    bool coming_back;
    int waiting_time_before_visit;
    double arrival_time;
    int fitting_time;
    bool is_in_fitting_room;
    int assigned_team;

} Person;

// Array of teams
int teams[4];

/**
 * Initializing teams to be all available (zero)
 */
void initialize_teams() {
    for (int i = 0; i < MAX_TEAM; i++) {    // Initializing the teams to zero=false=free
        teams[i] = 0;
    }
}

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

/**
 * Fuunction to sort an array of person structs in ascending order 
 * of their arrival time. 
 * @params the person list which is either a pirate array or a ninja array
 */
void sort_list(Person person_list[]) {
    int i, j, n;
    Person a;

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

/**
 * This function takes an average time/value and adds a variance
 * that is a generated random number with an approximate normal distribution
 * @param the average time or value
 * @return the average with the added variance
 */
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

/**
 * Tracks the elapsed time for given person
 * @param person is pirate or ninja
 */
void track_waiting_time(Person person) {
    time_t start_time = time(0);
    while (1) {
        person.waiting_time_before_visit = time(0) - start_time;
    }
    return;
}

/**
 * Determines if a thread/pirate/ninja is coming back or not.
 * The probability of person coming back is 25%
 * @return bool value. If false person is not coming back. If true person is coming back
 */
bool is_coming_back() {
    srand(time(0));
    double random_number = rand() % 4;
    if (random_number == 0) {
        return true;
    }
    return false;
}

/**
 * Compares the waiting time of the node at the front of the queue and returns a flag char
 * @return char 'n' refers to ninja and 'p' refers to pirate. -1 is when both queues are empty
 */
char compare_waiting_time() {
    if (pirate_queue->front == NULL && ninja_queue->front == NULL) {
        printf("Both queues are empty\n");
        return -1;
    }
    else if (pirate_queue->front->current_person.waiting_time_before_visit >= 
             ninja_queue->front->current_person.waiting_time_before_visit) {
        return 'p';
    }
    else {
        return 'n';
    }
}


void enter_fitting_room(int num_teams) {

    Person person_list[num_teams];
    char fitting_room_flag = NULL;
    int is_fitting_room_used = 0; // In use: 1; Empty: 0
    int i = 0;
    while (i < num_teams) {
        is_fitting_room_used = teams[i] || is_fitting_room_used;
    }
    if (is_fitting_room_used == 0) {
        
        fitting_room_flag = compare_waiting_time();
        
        if (fitting_room_flag == 'p') {
            for (i = 0; i < num_teams; i++) {
                Person *pirate = deQueue(pirate_queue);
                pirate->is_in_fitting_room = true;
                pirate->assigned_team = i;
                teams[i] = 1;
            }
        }
        else if (fitting_room_flag == 'n') {
            for (i = 0; i < num_teams; i++) {
                Person *ninja = deQueue(ninja_queue);
                ninja->is_in_fitting_room = true;
                ninja->assigned_team = i;
                teams[i] = 1;
            }
        } 
        else {
            printf("No flag for fitting room\n");
            // Not sure whta to do in this case
        }      
    }
}

/**
 * It constatnly moves person from queue to fitting room as long as the queues are not empty.
 * It runs the process two times to account for returning persons
 */
void fitting_room(int num_teams, int counter) {
    // Does this count for the ones that come back?
    counter += 1;
    sleep(2); // Waiting to make sure persons have entered the queue
    while (!((pirate_queue->front = NULL) && (ninja_queue->front = NULL))) {
        enter_fitting_room(num_teams);
    }
    if (counter == 2) {
        return;
    }
    fitting_room(num_teams, counter);
    return;
}


/**
 * Constantly checks for the fiiting room status. As soon as person is in fitting room
 * it waits for the period of fitting and either returns back to queue or gets destroyed
 * @param personis a ninja or pirate
 */
void check_thread_status(Person person) {
    while (1) {
        if (person.is_in_fitting_room == true) {
            sleep(person.fitting_time);
            person.is_in_fitting_room = false;
            teams[person.assigned_team] = 0;
            if (is_coming_back() == false) {
                //thread is done and out
            }
            else {
                //thread goes back outside ???
            }
        }
    }
    return;
}

/**
 * Adds person to waiting room queue based on arrival time
 * person is pirate or ninja
 * @return pointer to void
 */
void *enter_queue(Person person) {
    
    sleep(person.arrival_time);
    if (person.flag == 'p') {
        enQueue(pirate_queue, person);
        track_waiting_time(person);
    }
    else if (person.flag = 'n') {
        enQueue(ninja_queue, person);
    }
    else {
        printf("Person is not fully initialized\n");
    }
    check_thread_status(person);
    return NULL;
}

/**
 * Takes in a Person struct and initializes the elements of the struct
 * to set the id, if the person is coming back or not, adds variance to arrival time
 * fitting time and initializes a thread for that person
 * @param the person id, average costume time, average arrival time and 
 * the Person struct instance
 */
void create_new_person(int id, int avg_costume_time, int avg_arrival_time, 
                    Person new_person) {
    //new_person.coming_back = is_coming_back();
    new_person.fitting_time = add_variance(avg_costume_time);
    new_person.arrival_time = add_variance(avg_arrival_time);
    new_person.is_in_fitting_room = false;
    pthread_create(&new_person.thread, NULL, enter_queue(new_person), NULL);
    return;
}
 
/**
 * Makes a  Person struct for each of the pirates, sets 
 * their flag to be identified as a pirate and adds them to a pirate list
 * @param number of pirates given, average pirate costume time, average pirate arrival time,
 * and the number of teams that are in the fitting room (all given through command line)
 */
void initialize_pirate_threads(int num_pirates, int avg_pirate_costume_time, 
                               int avg_pirate_arrival_time, int num_teams) {

    for (int i = 0; i < (num_pirates);  i++) {
        Person new_pirate;
        new_pirate.flag = 'p';
        create_new_person(i, avg_pirate_costume_time, avg_pirate_arrival_time, new_pirate);
        Queue* pirate_queue = createQueue();

    }
    return;
}

/**
 * Creates a  Person struct for each of the ninjas, sets 
 * their flag to be identified as a ninja and adds them to a ninja list
 * @param number of ninjas given, average ninja costume time, average ninja arrival time,
 * and the number of teams that are in the fitting room (all given through command line)
 */
void initialize_ninja_threads(int num_ninjas, int avg_ninja_costume_time, 
                            int avg_ninja_arrival_time, int num_teams) {

    for (int i = 51; i < (num_ninjas + 51); i++) {
        Person new_ninja;
        new_ninja.flag = 'n';
        create_new_person(i, avg_ninja_costume_time, avg_ninja_arrival_time, new_ninja);
        Queue* ninja_queue = createQueue();
    }
    return;
}

/**
 * This function takes in the command line arguments and sets them to descriptive variables,
 * performs error checking for the given arguments and initializes the pirates and the ninjas
 * @param argc the number of command line arguments, argv the arguments themselves
 */
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
    FITTING_ROOM(num_teams);
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
