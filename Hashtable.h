// Copyright 2020 Dragne Lavinia-Stefana, Vasilache Raluca
#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#include "LinkedList.h"
#include "Queue.h"

#define DIE(assertion, call_description)                        \
    do {                                                        \
        if (assertion) {                                        \
            fprintf(stderr, "(%s:%u): ", __FILE__, __LINE__);   \
            perror(call_description);                           \
            exit(EXIT_FAILURE);                                 \
        }                                                       \
    } while (0)

struct info {
    int key;
    void *value;
};

struct article_info_list;

typedef struct article_info_list article_list;

struct article_info_list_reff;

typedef struct article_info_list_reff article_reff;

struct article_info_list_venue;

typedef struct article_info_list_venue article_venue;

struct article_info_list_field;

typedef struct article_info_list_field article_field;

struct article_info_list_author;

typedef struct article_info_list_author article_author;

struct article_info;

typedef struct article_info article;

struct Hashtable {
    LinkedList *buckets;
    int size;
    int hmax;
};

typedef struct Hashtable Hashtable;

Hashtable *init_ht(void);

void put(Hashtable *ht, int key, void *data);

void put_venue(Hashtable *ht, void* key, void *data);

void put_reff(Hashtable *ht, int key, void *data);

void put_field(Hashtable *ht, void* key, void *data);

void put_author(Hashtable *ht, int64_t key, void *data);

void add_ht_list(Hashtable *data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, const int64_t id, const int64_t* references,
    const int num_refs);

void add_ht_list_reff(Hashtable *data, const int64_t id);

void add_ht_list_venue(Hashtable *data, const char* venue, const int64_t id);

void add_ht_list_field(Hashtable *data, const char* field, const int64_t id);

void add_ht_list_author(Hashtable *data, int64_t id_author,
    int64_t id_paper, int year);

void destroy_article_list(article_list *data);

void free_ht(Hashtable *ht);

void free_ht_venue(Hashtable *ht);

void free_ht_field(Hashtable *ht);

void free_ht_reff(Hashtable *ht);

void free_ht_author(Hashtable *ht);

Node* get_reff(Hashtable *ht_list, int64_t id);

Node* get_node(Hashtable *ht_list, int64_t id);

void clear_visited_node(Hashtable *ht, int64_t id);

void visit(Node *curr_ref,  Queue *q);

void get_min_year_article(Node *curr_ref, int *min_year, int64_t *min_id,
    int *max_freq, char **name, Hashtable *reff);

char *bfs_list_ref(Hashtable *lg, Hashtable *ref, int64_t id);

unsigned int hash_function_int(void *a);

unsigned int hash_function_string(void *a);

#endif
