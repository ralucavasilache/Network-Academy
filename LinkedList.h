// Copyright 2020 Dragne Lavinia-Stefana, Vasilache Raluca
// schelet preluat din laborator
#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

typedef struct Node {
    /* Pentru ca datele stocate sa poata avea
    orice tip, folosim un pointer la void. */
    void *data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
} LinkedList;

struct info_list;

void init_list(LinkedList *list);

void add_nth_node(LinkedList *list, int n, void *new_data);

void add_nth_node_alloc(LinkedList *list, int n,
    void *new_data, int value_size_bytes);

Node* remove_nth_node(LinkedList *list, int n);

int get_size(LinkedList *list);

void free_list(LinkedList **list);

#endif /* __LINKEDLIST_H__ */
