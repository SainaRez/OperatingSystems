#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct data {
    void *data;
    struct data *next;
} data;

typedef struct queue {
    unsigned int length;
    size_t allocation_size; // The size of things being stored in data
    data *head;
    data *tail;
} queue;

queue *create_queue(size_t alloc_size);

void enqueue(queue *q, void *data);

void *dequeue(queue *q);

void peek(queue *q, void *to_return);//Return the first element

void clear_queue(queue *q);

void destroy_queue(queue *q);

unsigned int get_length(queue *q);

void process_queue(queue *q, void *func(void *));

bool is_empty(queue *q);
