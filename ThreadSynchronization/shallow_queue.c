#include "shallow_queue.h"



// A utility function to create a new QNode.
QNode *newNode(person *p) {
    QNode *temp = (struct QNode *) malloc(sizeof(struct QNode));
    temp->current_person = p;
    temp->next = NULL;
    return temp;
};

// A utility function to create an empty queue
shallow_queue *createQueue() {
    shallow_queue *q = (shallow_queue *) malloc(sizeof(shallow_queue));
    q->front = q->rear = NULL;
    return q;
}

// The function to add a key k to q
void enQueue(shallow_queue *q, person *p) {
    // Create a new LL node
    //struct QNode* temp = newNode(k);
    QNode *temp = newNode(p);

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
person *deQueue(shallow_queue *q) {
    // If queue is empty, return NULL.
    if (q->front == NULL)
        return NULL;

    // Store previous front and move front one node ahead
    QNode *temp = q->front;
    person *node_struct = temp->current_person;

    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;

    return node_struct;
}
