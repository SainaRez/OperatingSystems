#pragma once

/**
 * Implementation of Queue
 */
typedef struct QNode {
    person *current_person;
    struct QNode *next;
} QNode;

typedef struct shallow_queue {
    struct QNode *front;
    struct QNode *rear;
} shallow_queue;


QNode *newNode(person *p);

shallow_queue *createQueue();

void enQueue(shallow_queue *q, person *p);

person *deQueue(shallow_queue *q);
