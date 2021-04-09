// Copyright 2020 Dragne Lavinia-Stefana, Vasilache Raluca
// schelet preluat din laborator
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"
#include "Hashtable.h"

/*
 * Functie care trebuie apelata dupa alocarea unei liste simplu inlantuite, pentru a o initializa.
 * (Setare valori initiale pentru campurile specifice structurii LinkedList).
 */
void init_list(LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}
/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e adaugat pe pozitia n a listei
 * reprezentata de pointerul list. Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla
 * pe pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei.
 */
void add_nth_node(LinkedList *list, int n, void *new_data) {
    Node *prev, *curr;
    Node *new_node;

    if (list == NULL) {
        return;
    }

    /* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
    if (n > list->size) {
        n = list->size;
    } else if (n < 0) {
        return;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("Not enough memory to add element!");
        exit(-1);
    }
    new_node->data = new_data;
    new_node->next = curr;
    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    if (new_node->next == NULL) {
        list->tail = new_node;
    }

    list->size++;
}
void add_nth_node_alloc(LinkedList *list, int n,
    void *new_data, int value_size_bytes) {
    Node *prev, *curr;
    Node *new_node;

    if (list == NULL) {
        return;
    }

    /* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
    if (n > list->size) {
        n = list->size;
    } else if (n < 0) {
        return;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("Not enough memory to add element!");
        exit(-1);
    }
    new_node->data = malloc(value_size_bytes);
    memcpy(new_node->data, new_data, value_size_bytes);
    new_node->next = curr;

    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    if (new_node->next == NULL) {
        list->tail = new_node;
    }

    list->size++;
}
/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca parametru.
 * Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1,
 * se elimina nodul de la finalul listei.
 * Functia intoarce un pointer spre acest nod proaspat eliminat din lista.
 * Este responsabilitatea apelantului sa elibereze memoria acestui nod.
 */
Node* remove_nth_node(LinkedList *list, int n) {
    Node *prev, *curr;

    if (list == NULL) {
        return NULL;
    }

    if (list->head == NULL) { /* Lista este goala. */
        return NULL;
    }

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
    if (n > list->size - 1) {
        n = list->size - 1;
    } else if (n < 0) {
        return NULL;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = curr->next;
    } else {
        prev->next = curr->next;
        if (prev->next == NULL) {
            list->tail = prev;
        }
    }

    list->size--;
    return curr;
}
/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca parametru.
 */
int get_size(LinkedList *list) {
    if (list == NULL) {
        return -1;
    }

    return list->size;
}
/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la sfarsit, elibereaza memoria folosita
 * de structura lista si actualizeaza la NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void free_list(LinkedList **pp_list) {
    struct Node *currNode;

    if (pp_list == NULL || *pp_list == NULL) {
        return;
    }

    while (get_size(*pp_list) > 0) {
        currNode = remove_nth_node(*pp_list, 0);
        free(currNode->data);
        free(currNode);
    }

    free(*pp_list);
    *pp_list = NULL;
}

