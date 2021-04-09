// Copyright 2020 Dragne Lavinia-Stefana, Vasilache Raluca
// schelet preluat din laborator
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "LinkedList.h"

typedef struct {
    LinkedList *list;
} Queue;

void init_q(Queue *q);

int get_size_q(Queue *q);

int is_empty_q(Queue *q);

void* front(Queue *q);

Node* dequeue(Queue *q);

void enqueue(Queue *q, void *new_data, int key_size_bytes);

void clear_q(Queue *q);

void purge_q(Queue *q);

#endif  // __QUEUE_H__
