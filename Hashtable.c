// Copyright 2020 Dragne Lavinia-Stefana, Vasilache Raluca
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "Hashtable.h"
#include "Queue.h"

#define SIZE 5003
#define N_Year 2060
#define MAX_FREQ 0
#define MIN_ID 10000000000
#define MIN_YEAR 10000

struct article_info_list {
    char* title;
    char* venue;
    int year;
    char** author_names;
    int64_t* author_ids;
    char** institutions;
    int num_authors;
    char** fields;
    int num_fields;
    int64_t id;
    int64_t* references;
    int num_refs;
    int visited;
    int freq;
};

struct article_info {
    char* title;
    char* venue;
    int year;
    char** author_names;
    int64_t* author_ids;
    char** institutions;
    int num_authors;
    char** fields;
    int num_fields;
    int64_t id;
    int64_t* references;
    int num_refs;
};
// hashtable care retine numarul de citari
struct article_info_list_reff {
    int64_t id;
    int freq;
};
// hashtable care foloseste venue drept cheie
struct article_info_list_venue {
    int64_t id;
    char* venue;
};
// hastbale care foloseste field drept cheie
struct article_info_list_field {
    int64_t id;
    char* field;
};
// cheie care foloseste id-ul autorului drept cheie
struct article_info_list_author {
    int64_t id_author;
    int64_t id_paper;
    int year;
};

unsigned int hash_function_int(void *a) {
    /*
     * Credits: https://stackoverflow.com/a/12996028/7883884
     */
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_string(void *a) {
    /*
     * Credits: http://www.cse.yorku.ca/~oz/hash.html
     */
    unsigned char *puchar_a = (unsigned char*) a;
    int64_t hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

    return hash;
}
// functie care initializeaza hashtbale-ul
Hashtable* init_ht(void) {
    // aloc un vector de liste
    Hashtable *ht = malloc(sizeof(Hashtable));
    DIE(ht == NULL, "ht malloc");
    ht->buckets  = malloc(SIZE * sizeof(LinkedList));
    DIE(ht->buckets == NULL, "ht->buckets malloc");
    int i;
    for (i = 0; i < SIZE; i++) {
        // o initializez fiecare lista din vector
        init_list(&ht->buckets[i]);
    }
    ht->hmax = SIZE;
    ht->size = 0;
    return ht;
}
// functie care adauga un nou nod in hashtable
void put(Hashtable *ht, int key, void *data) {
    int index = hash_function_int(&key) % ht->hmax;
    // pointez pe primul element din lista corespunzatoare
    struct Node *curr = ht->buckets[index].head;
    if (curr != NULL) {
    while (curr != NULL && (((article_list *)curr->data)->id) != key)
        curr = curr->next;
    }
    // daca nu l-am gasit trebuie adaugat
    if (curr == NULL)  {
        // adaug nodul cu "valoarea"<=> "data", new,
        // in lista buckets[index] la final
        add_nth_node(&ht->buckets[index], ht->buckets[index].size,
            ((article_list *) data));
        ht->size++;
    }

    return;
}
void put_reff(Hashtable *ht, int key, void *data) {
    int index = hash_function_int(&key) % ht->hmax;
    // pointez pe primul element din lista corespunzatoare
    struct Node *curr = ht->buckets[index].head;
    if (curr != NULL) {
    while (curr != NULL && (((article_reff *)curr->data)->id) != key)
        curr = curr->next;
    }
    // daca nu l-am gasit trebuie adaugat
    if (curr == NULL)  {
        add_nth_node(&ht->buckets[index], ht->buckets[index].size,
            (article_reff *) data);
        ht->size++;
    }

    return;
}

void put_venue(Hashtable *ht, void* key, void *data) {
    int index = hash_function_string(key) % ht->hmax;
    // pointez pe primul element din lista corespunzatoare
    struct Node *curr = ht->buckets[index].head;

    while (curr != NULL)
        curr = curr->next;
    // il adaug pe locul disponibil
    add_nth_node(&ht->buckets[index], ht->buckets[index].size,
        (article_venue *)data);
    ht->size++;

    return;
}

void put_field(Hashtable *ht, void* key, void *data) {
    int index = hash_function_string(key) % ht->hmax;
    // pointez pe primul element din lista corespunzatoare
    struct Node *curr = ht->buckets[index].head;

    while (curr != NULL)
        curr = curr->next;
    // il adaug pe locul disponibil
    add_nth_node(&ht->buckets[index], ht->buckets[index].size,
        (article_field *) data);
    ht->size++;

    return;
}

void put_author(Hashtable *ht, int64_t key, void *data) {
    int index = hash_function_int(&key) % ht->hmax;
    // pointez pe primul element din lista corespunzatoare
    struct Node *curr = ht->buckets[index].head;

    while (curr != NULL)
        curr = curr->next;
    // il adaug pe locul disponibil
    add_nth_node(&ht->buckets[index], ht->buckets[index].size,
        (article_author *)data);
    ht->size++;

    return;
}
void add_ht_list(Hashtable *data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, const int64_t id, const int64_t* references,
    const int num_refs) {

    int i;
    article_list *value = malloc(sizeof(article_list));
    DIE(value == NULL, "value malloc");

    article_list *new_node;
    new_node = value;
    // alocare si initializare title
    new_node->title = malloc((strlen(title)+1)*sizeof(char));
    DIE(new_node->title == NULL,
        "new_node->title malloc");
    snprintf(new_node->title, strlen(title) + 1, "%s", title);
    // alocare si initializare venue
    new_node->venue = malloc((strlen(venue)+1)*sizeof(char));
    DIE(new_node->venue == NULL,
        "new_node->venue malloc");
    snprintf(new_node->venue, strlen(venue) + 1, "%s", venue);
    // alocare author_names (vector de string-uri)
    new_node->author_names = malloc(num_authors*sizeof(char *));
    DIE(new_node->author_names == NULL,
        "new_node->author_names malloc");
    // alocare si initializare nume pentru fiecare autor
    for (i = 0; i < num_authors; i++) {
        new_node->author_names[i] = malloc((strlen(author_names[i])+1));
        DIE(new_node->author_names[i] == NULL,
            "new_node->author_names[i] malloc");
        snprintf(new_node->author_names[i],
            strlen(author_names[i]) + 1, "%s", author_names[i]);
    }
    // alocare author_ids (vector de int-uri)
    new_node->author_ids = malloc((num_authors)*sizeof(int64_t));
    DIE(new_node->author_ids == NULL,
        "new_node->author_ids malloc");
    // initializare id pentru fiecare autor
    for (i = 0; i < num_authors; i++) {
        new_node->author_ids[i] = author_ids[i];
    }
    // alocare institutions (vector de string-uri)
    new_node->institutions = malloc(num_authors*sizeof(char *));
    DIE(new_node->institutions == NULL,
        "new_node->institutions malloc");
    // alocare si initializare fiecare institutie in parte
    for (i = 0; i < num_authors; i++) {
        new_node->institutions[i] = malloc((strlen(institutions[i])+1));
        DIE(new_node->institutions[i] == NULL,
            "institutions[i] malloc");
        snprintf(new_node->institutions[i],
            strlen(institutions[i]) + 1, "%s", institutions[i]);
    }
    // alocare fields (vector de string-uri)
    new_node->fields = malloc(num_fields*sizeof(char *));
    DIE(new_node->fields == NULL,
        "new_node->fields malloc");
    // alocare si initializare fiecare field
    for (i = 0; i < num_fields; i++) {
        new_node->fields[i] = malloc((strlen(fields[i])+1)*sizeof(char));
        DIE(new_node->fields[i] == NULL,
            "fields[i] malloc");
        snprintf(new_node->fields[i],
            strlen(fields[i]) + 1, "%s", fields[i]);
    }
    // alocare references (vector de int-uri)
    new_node->references = malloc(num_refs*sizeof(int64_t));
    DIE(new_node->references == NULL,
        "new_node->references malloc");
    // initializare references
    for (i = 0; i < num_refs; i++) {
        new_node->references[i] = references[i];
    }

    new_node->year = year;
    new_node->num_authors = num_authors;
    new_node->num_fields = num_fields;
    new_node->id = id;
    new_node->num_refs = num_refs;
    new_node->visited = 0;
    new_node->freq = 0;
    // adaugare structura in hashtable
    put(data, new_node->id, value);
}
// functie care aloca un nou element care urmeaza sa fie adaugat
// si completeaza campurile asociate
void add_ht_list_reff(Hashtable *data, const int64_t id) {
    article_reff *value = malloc(sizeof(article_reff));
    DIE(value == NULL, "value malloc");

    article_reff *new_node = value;

    new_node->id = id;
    new_node->freq = 1;

    put_reff(data, new_node->id, value);
}

void add_ht_list_venue(Hashtable *data, const char* venue, const int64_t id) {
    article_venue *value = malloc(sizeof(article_venue));
    DIE(value == NULL, "value malloc");

    article_venue *new_node = value;

    new_node->id = id;
    new_node->venue = malloc((strlen(venue)+1)*sizeof(char));
    DIE(new_node->venue == NULL, "new_node->venue malloc");
    snprintf(new_node->venue, strlen(venue) + 1, "%s", venue);

    put_venue(data, new_node->venue, value);
}

void add_ht_list_field(Hashtable *data, const char* field, const int64_t id) {
    article_field *value = malloc(sizeof(article_field));
    DIE(value == NULL, "value malloc");

    article_field *new_node = value;

    new_node->id = id;
    new_node->field = malloc((strlen(field)+1)*sizeof(char));
    DIE(new_node->field == NULL, "new_node->field malloc");
    snprintf(new_node->field, strlen(field) + 1, "%s", field);

    put_field(data, new_node->field, value);
}

void add_ht_list_author(Hashtable *data, int64_t id_author,
    int64_t id_paper, int year) {
    article_author *value = malloc(sizeof(article_author));
    DIE(value == NULL, "value malloc");

    article_author *new_node = value;

    new_node->id_author = id_author;
    new_node->id_paper = id_paper;
    new_node->year = year;

    put_author(data, new_node->id_author, value);
}

// functie care elibereaza
// o structura de tip article_list
// si campurile asociate ei
void destroy_article_list(article_list *data) {
    int i;
    free(data->title);
    free(data->venue);

    for (i = 0; i < data->num_authors; i++) {
        free(data->author_names[i]);
    }

    free(data->author_names);
    free(data->author_ids);

    for (i = 0; i < data->num_authors; i++) {
        free(data->institutions[i]);
    }

    free(data->institutions);
    free(data->references);

    for (i = 0; i < data->num_fields; i++) {
        free(data->fields[i]);
    }
    free(data->fields);
}

void free_ht(Hashtable *ht) {
    int i;
    // parcurg vectorul de buckets
    for (i = 0; i < ht->hmax; i++) {
        // eliberez fiecare lista. daca nu e goala
        while (ht->buckets[i].size > 0) {
            // sterg datele head-ului listei[i]
            destroy_article_list(((article_list*) ht->buckets[i].head->data));
            free(((article*)ht->buckets[i].head->data));
            // sterg head-ul listei, cat timp are head
            struct Node * head_node = remove_nth_node(&(ht->buckets[i]), 0);
            free(head_node);
            ht->size--;
        }
    }
    // eliberez vectorul
    free(ht->buckets);
    // eliberez hashtable
    free(ht);
}

void free_ht_venue(Hashtable *ht) {
    int i;
    // parcurg vectorul de buckets
    for (i = 0; i < ht->hmax; i++) {
        // eliberez fiecare lista. daca nu e goala
        while (ht->buckets[i].size > 0) {
            free(((article_venue*)ht->buckets[i].head->data)->venue);
            free(((article*)ht->buckets[i].head->data));
            // sterg head-ul listei, cat timp are head
            struct Node * head_node = remove_nth_node(&(ht->buckets[i]), 0);
            free(head_node);
            ht->size--;
        }
    }
    // eliberez vectorul
    free(ht->buckets);
    // eliberez hashtable
    free(ht);
}

void free_ht_field(Hashtable *ht) {
    int i;
    // parcurg vectorul de buckets
    for (i = 0; i < ht->hmax; i++) {
        // eliberez fiecare lista. daca nu e goala
        while (ht->buckets[i].size > 0) {
            free(((article_field*)ht->buckets[i].head->data)->field);
            free(((article*)ht->buckets[i].head->data));
            // sterg head-ul listei, cat timp are head
            struct Node * head_node = remove_nth_node(&(ht->buckets[i]), 0);
            free(head_node);
            ht->size--;
        }
    }
    // eliberez vectorul
    free(ht->buckets);
    // eliberez hashtable
    free(ht);
}

void free_ht_reff(Hashtable *ht) {
    int i;
    // parcurg vectorul de buckets
    for (i = 0; i < ht->hmax; i++) {
        // eliberez fiecare lista. daca nu e goala
        while (ht->buckets[i].size > 0) {
            free(((article*)ht->buckets[i].head->data));
            // sterg head-ul listei, cat timp are head
            struct Node * head_node = remove_nth_node(&(ht->buckets[i]), 0);
            free(head_node);
            ht->size--;
        }
    }
    // eliberez vectorul
    free(ht->buckets);
    // eliberez hashtable
    free(ht);
}

void free_ht_author(Hashtable *ht) {
    int i;
    // parcurg vectorul de buckets
    for (i = 0; i < ht->hmax; i++) {
        // eliberez fiecare lista. daca nu e goala
        while (ht->buckets[i].size > 0) {
            free(((article_author*)ht->buckets[i].head->data));
            // sterg head-ul listei, cat timp are head
            struct Node * head_node = remove_nth_node(&(ht->buckets[i]), 0);
            free(head_node);
            ht->size--;
        }
    }
    // eliberez vectorul
    free(ht->buckets);
    // eliberez hashtable
    free(ht);
}

// functie care cauta un paper in ht_reff
Node* get_reff(Hashtable *ht_list, int64_t id) {
    int index = hash_function_int(&id)%SIZE;
    struct Node *curr = ht_list->buckets[index].head;
    if (curr != NULL) {
        while (curr != NULL && id != ((article_reff *)curr->data)->id) {
            curr = curr->next;
        }
    }

    return curr;
}
// functie care cauta un paper in ht_list
Node* get_node(Hashtable *ht_list, int64_t id) {
    int index = hash_function_int(&id)%SIZE;
    struct Node *curr = ht_list->buckets[index].head;
    if (curr != NULL) {
        while (curr != NULL && id != ((article_list *)curr->data)->id) {
            curr = curr->next;
        }
    }

    return curr;
}
// functie care reseteaza valorile pentru visited
void clear_visited_node(Hashtable *ht, int64_t id) {
    int j;
    // ma pozitionez in lista in care ar trebui sa fie
    struct Node *curr = get_node(ht, id);
    // daca curr este NULL
    // inseamna ca papper-ul n-a fost adaugat
    // si nici altii cu acelasi hash ca el
    if (curr != NULL) {
        if (((article_list*)curr->data)->visited != 0) {
            ((article_list*)curr->data)->visited = 0;
        }

        for (j = 0; j < ((article_list*)curr->data)->num_refs; j++) {
            // ii caut vecinii
            int64_t find_id = ((article_list*)curr->data)->references[j];
            struct Node *curr_ref = get_node(ht, find_id);
            // curr_ref este NULL, vecinul nu a fost adaugat
            // daca a fost adaugat, apelez recursiv pentru
            // toate paper-urile de care depinde
            if (curr_ref != NULL) {
                if (((article_list*)curr_ref->data)->visited != 0) {
                    int64_t x = ((article_list*)curr->data)->references[j];
                    clear_visited_node(ht, x);
                }
            }
        }
    }

    return;
}
// functie care marcheaza un articol ca vizitat
void visit(Node *curr_ref,  Queue *q) {
    ((article_list*)(curr_ref->data))->visited = 1;
    enqueue(q, ((article_list *)curr_ref->data), sizeof(article_list));
}
// functie care cauta articolul cu anul minim
// (cu numarul de citari maxim sau id-ul minim
// daca este cazul)
void get_min_year_article(Node *curr_ref, int *min_year, int64_t *min_id,
    int *max_freq, char **name, Hashtable *reff) {

    int64_t id_vecin = ((article_list*)(curr_ref->data))->id;
    struct Node *back = get_reff(reff, id_vecin);
    // daca se gaseste un articol cu anul mai mic
    // se actualizeaza variabilele
    if (((article_list*)(curr_ref->data))->year < *min_year) {
        *min_year = ((article_list*)curr_ref->data)->year;
        *name = ((article_list*)(curr_ref->data))->title;
        *max_freq = ((article_reff*)(back->data))->freq;
        *min_id = ((article_reff*)(back->data))->id;
    }
    // daca se gaseste un articol cu anul egal
    // trebuie tinut cont de numarul de citari
    if (((article_list*)(curr_ref->data))->year == *min_year) {
        if ((((article_reff*)(back->data))->freq) > *max_freq) {
            *max_freq = ((article_reff*)(back->data))->freq;
            *min_id = ((article_reff*)(back->data))->id;
            *name = ((article_list*)(curr_ref->data))->title;
        } else {
            if ((((article_reff*)(back->data))->freq) == *max_freq) {
                if ((((article_reff*)(back->data))->id) < *min_id) {
                    *min_id = ((article_reff*)(back->data))->id;
                    *name = ((article_list*)(curr_ref->data))->title;
                }
            }
        }
    }
}

char *bfs_list_ref(Hashtable *ht, Hashtable *reff, int64_t id) {
    int i;
    int min_year = MIN_YEAR;
    int max_freq = MAX_FREQ;
    int64_t min_id = MIN_ID;

    char *name = "None";

    Queue *q = malloc(1*sizeof(Queue *));
    if (q == NULL) {
        return NULL;
    }
    init_q(q);
    // cautare paper
    struct Node *curr = get_node(ht, id);

    // l-am gasit
    if (curr != NULL) {
        // il adaug in coada
        enqueue(q, ((article_list *)curr->data), sizeof(article_list));
        // cat timp coada nu e goala
        while (q->list->size > 0) {
            // scot un nod
            Node *v = dequeue(q);
            // il caut in ht
            int64_t find_id = ((article_list *)v->data)->id;
            // ma pozitionez in lista in care ar trebui sa fie
            struct Node *curr_r = get_node(ht, find_id);
            // am gasit nodul din coada in ht
            if (curr_r != NULL) {
                // bag toti vecinii lui in coada
                int num_refs = ((article_list*)curr_r->data)->num_refs;
                for (i = 0; i < num_refs; i++) {
                    int64_t x = ((article_list*)curr_r->data)->references[i];
                    // caut vecinii in ht
                    struct Node *curr_ref = get_node(ht, x);
                    // daca curr_reff este NULL,
                    // vecinul nu au fost inca adaugat
                    // daca a fost adaugat il bag in coada
                    if (curr_ref != NULL) {
                        // daca e nevizitat
                        if (((article_list*)curr_ref->data)->visited == 0) {
                            // il vizitez
                            visit(curr_ref, q);


                            // actualizare oldest_influence cu cele
                            // mai multe citari
                            get_min_year_article(curr_ref, &min_year,
                                &min_id, &max_freq, &name, reff);
                        }
                    }
                }
                // daca gasisem nodul in coada il marchez
                if (curr_r != NULL)
                    ((article_list*)curr_r->data)->visited = 2;
            }
            free(v->data);
            free(v);
        }
    } else {
        free(q->list);
        free(q);
        return name;
    }

    purge_q(q);
    free(q);
    return name;
}


