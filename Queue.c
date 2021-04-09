// Copyright 2020 Dragne Lavinia-Stefana, Vasilache Raluca
// schelet preluat din laborator
#include <stdlib.h>
#include <stdio.h>

#include "Queue.h"

void init_q(Queue *q) {
    q->list = malloc(sizeof(LinkedList));
    if (q == NULL) {
        perror("Not enough memory to initialize the queue!");
        exit(-1);
    }

    init_list(q->list);
}

int get_size_q(Queue *q) {
    return q->list->size;
}

int is_empty_q(Queue *q) {
    return get_size_q(q) == 0;
}

void* front(Queue *q) {
    if (q == NULL || q->list == NULL || q->list->head == NULL) {
        return NULL;
    }

    return q->list->head->data;
}

Node* dequeue(Queue *q) {
    struct Node *node;
    if (q == NULL || q->list == NULL) {
        return NULL;
    }

    node = remove_nth_node(q->list, 0);
    return node;
}

void enqueue(Queue *q, void *new_data, int key_size) {
    add_nth_node_alloc(q->list, q->list->size, new_data, key_size);
}

void clear_q(Queue *q) {
    struct Node *node;
    while (!is_empty_q(q)) {
        node = remove_nth_node(q->list, 0);
        free(node->data);
        free(node);
    }
}


void purge_q(Queue *q) {
    clear_q(q);
    free(q->list);
}
