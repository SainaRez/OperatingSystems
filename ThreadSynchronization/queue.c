#include "queue.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Create and return an empty queue
 *
 * @param alloc_size
 * @return
 */
queue *create_queue(size_t alloc_size) {
    queue *q = (queue *) malloc(sizeof(queue));
    if (q == NULL) {
        fprintf(stderr, "Error: malloc failed to allocate queue memory");
        return NULL;
    }
    q->allocation_size = alloc_size;
    q->length = 0;
    q->head = q->tail = NULL;
    return q;
}

/**
 * Insert data into the queue(last position)
 *
 * Given queue cannot be NULL.
 *
 * @param q The queue to insert data into
 * @param _data A pointer to the data
 */
void enqueue(queue *q, void *_data) {
    assert(q != NULL);

    struct data *to_insert = (struct data *) malloc(sizeof(data)); // TODO could this right side just be a void *
    if (to_insert == NULL) {
        fprintf(stderr, "Error allocating memory");
        exit(-1);
    }
    to_insert->data = malloc(sizeof(q->allocation_size));
    if (to_insert->data == NULL) {
        fprintf(stderr, "Error allocating memory");
        exit(-1);
    }
    to_insert->next = NULL;
    memcpy(to_insert->data, _data, q->allocation_size);
    if (q->length == 0) { // First insertion
        q->head = q->tail = to_insert;
    } else {
        q->tail->next = to_insert;
        q->tail = to_insert;
    }

    q->length++;
}


/**
 * Removes the first element of the queue and returns a pointer to its data
 *
 * q cannot be null or empty
 *
 * @param q Queue to pop from
 */
void *dequeue(queue *q) {
    assert(q != NULL);
    assert(is_empty(q) == false);

    data *to_delete = q->head->data;
    if (q->length == 1) {
        q->head = q->tail = NULL;
        q->length--;
    } else {
        q->head = q->head->next;
        q->length--;
    }

    return to_delete;
}


/*
Save first element of the queue to the to_return argument
*/
/**
 * Copies the contents of the front of the queue to the to_return parameter
 * @param q Queue in consideration
 * @param to_return Pointer to location where the memory of the front will be copied to
 */
void peek(queue *q, void *to_return) {
    assert(q != NULL);

    memcpy(to_return, q->head->data, q->allocation_size);
}


/**
 * Deletes all the data inside a queue and frees elements
 * @param q Queue to clear
 */
void clear_queue(queue *q) {
    assert(q != NULL);

    while (!is_empty(q)) {
        data *temp = q->head;
        q->head = q->head->next;
        free(temp->data);
        free(temp);
        q->length--;
    }
}

/**
 *
 * @param q Queue to get length of
 * @return Number of elements in the queue
 */
unsigned int get_length(queue *q) {
    if (q == NULL) {
        fprintf(stderr, "Illegal argument exception, null q provided\n");
        exit(EXIT_FAILURE);
    }

    return q->length;
}

/**
 * Check is queue is empty
 * @param q Queue to check status of
 * @return True if queue is empty or NULL
 */
bool is_empty(queue *q) {
    if (q == NULL) {
        return true;
    }

    return (q->length == 0);
}


/**
 * Runs a given function on all data on all elements in the queue
 * @param queue Queue to process
 * @param func A function which will run on each data element of the queue
 */
void process_queue(queue *q, void *func(void *)) {
    assert(q != NULL);
    
    struct data *current = q->head;
    while (current != NULL) {
        data *next = current->next;
        func(current->data);
        current = next;
    }
}

/**
 * Empties the queue and frees its memory
 * @param q Pointer to the queue to destroy
 */
void destroy_queue(queue *q) {
    clear_queue(q);
    free(q);
}

