// Copyright 2020 Dragne Lavinia-Stefana, Vasilache Raluca
#include "publications.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define N_MAX 5003
#define N_Year 2060
#define MAX_AUTHORS 100

#include "Hashtable.h"
#include "Queue.h"

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

struct article_info_list_reff {
    int64_t id;
    int freq;
};

struct article_info_list_venue {
    int64_t id;
    char* venue;
};

struct article_info_list_field {
    int64_t id;
    char* field;
};

struct article_info_list_author {
    int64_t id_author;
    int64_t id_paper;
    int year;
};

struct publications_data {
    int *freq;
    Hashtable *ht_list;
    Hashtable *ht_reff;
    Hashtable *ht_venue;
    Hashtable *ht_field;
    Hashtable *ht_author;
};

PublData* init_publ_data(void) {
    PublData *data = malloc(sizeof(PublData));
    DIE(data == NULL, "data malloc");

    data->freq = calloc(N_Year, sizeof(int));
    data->ht_list = init_ht();
    data->ht_reff = init_ht();
    data->ht_venue = init_ht();
    data->ht_field = init_ht();
    data->ht_author = init_ht();

    return data;
}

void destroy_publ_data(PublData *data) {
    free(data->freq);
    free_ht(data->ht_list);
    free_ht_reff(data->ht_reff);
    free_ht_venue(data->ht_venue);
    free_ht_field(data->ht_field);
    free_ht_author(data->ht_author);
    free(data);
}
// functie care cauta un articol cu id-ul id in ht_list
Node* get_ht_list(Hashtable *ht_list, int64_t id) {
    int index = hash_function_int(&id)%N_MAX;
    struct Node *curr = ht_list->buckets[index].head;
    if (curr != NULL) {
        while (curr != NULL && id != ((article *)curr->data)->id) {
            curr = curr->next;
        }
    }
    return curr;
}
// functie care cauta un articol cu id-ul id in ht_reff
Node* get_ht_list_reff(Hashtable *ht_list, int64_t id) {
    int index = hash_function_int(&id)%N_MAX;
    struct Node *curr = ht_list->buckets[index].head;
    if (curr != NULL) {
        while (curr != NULL && id != ((article_reff *)curr->data)->id) {
            curr = curr->next;
        }
    }
    return curr;
}
// functie care cauta un autor in ht_author
int find_author(int64_t *v_authors, int64_t id_author, int n_authors) {
    int gasit = 0, i;
    for (i = 0; i < n_authors; i++) {
        if (v_authors[i] == id_author) {
            gasit = 1;
            break;
        }
    }

    return gasit;
}
// functie care returneaza numarul total de autori
// care apartin unei institutii si au scris un articol
// care studiaza domeniul venue
int get_ht_list_field(Hashtable *ht_list, Hashtable *ht_field,
    char* institution, char* field, int64_t *v_authors) {

    int total_auth = 0;
    int index = hash_function_string(field)%N_MAX;
    struct Node *curr = ht_field->buckets[index].head;
    while (curr != NULL) {
        // am gasit un articol cu acel field
        if (strcmp(field, ((article_field *)curr->data)->field) == 0) {
            int64_t id = ((article_field *)curr->data)->id;
            // il caut dupa id in ht-ul mare
            struct Node* new = get_ht_list(ht_list, id);

            // caut in lista de institution ai lui new
            int i;
            for (i = 0; i < ((article *)new->data)->num_authors; i++) {
                // daca institution[i] == institution
                char *aux = ((article *)new->data)->institutions[i];
                if (strcmp(aux, institution) == 0) {
                    int64_t x = ((article *)new->data)->author_ids[i];
                    int ok = find_author(v_authors, x, total_auth);
                    // nu este in vectorul de autori
                    // il adaug
                    if (ok == 0) {
                        int64_t id = ((article *)new->data)->author_ids[i];
                        v_authors[total_auth] = id;
                        total_auth++;
                    }
                }
            }
            // caut author[i] in vectorul de autori
            // daca nu a mai fost adaugat, il adaug
            // daca a mai fost adaugat il ignor
        }
        curr = curr->next;
    }
    return total_auth;
}

// functie care carculeaza venue_impact_factor
float get_venue(Hashtable *ht_venue, Hashtable *ht_reff, char* venue) {
    int total_venue = 0;
    int citations = 0;
    int index = hash_function_string(venue)%N_MAX;
    struct Node *curr = ht_venue->buckets[index].head;
        // caut un articol cu acel venue
        while (curr != NULL) {
            // am gasit
            if (strcmp(venue, ((article_venue *)curr->data)->venue) == 0) {
                total_venue++;
                // il caut in reff
                int64_t id = ((article_reff *) curr->data)->id;
                struct Node *curr_reff = get_ht_list_reff(ht_reff, id);
                // daca l-am gasit
                if (curr_reff != NULL) {
                    // maresc numarul de citari
                    citations += ((article_reff *)curr_reff->data)->freq - 1;
                }
            }
            curr = curr->next;
        }
    // calculare medie
    if (total_venue != 0) {
        return ((float)(citations) / (float)(total_venue));
    } else {
        return 0;
    }
}

void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, const int64_t id, const int64_t* references,
    const int num_refs) {
    int i;

    // completare vector de frecventa pentru ani
    data->freq[year]++;

    // adaugare articole in ht_list
    add_ht_list(data->ht_list, title, venue,
    year, author_names, author_ids,
    institutions, num_authors, fields,
    num_fields, id, references, num_refs);

    // adaugare articole in ht_venue
    add_ht_list_venue(data->ht_venue, venue, id);

    for (i = 0; i < num_fields; i++) {
        add_ht_list_field(data->ht_field, fields[i], id);
    }

    struct Node *curr_reff = get_ht_list_reff(data->ht_reff, id);
    // daca nu e adaugat il adaug
    if (curr_reff == NULL) {
        add_ht_list_reff(data->ht_reff, id);
    // daca e, ii cresc numarul de citari
    } else {
        ((article_list_reff *)curr_reff->data)->freq++;
    }

    // merg prin lista de vecini
    for (i = 0; i < num_refs; i++) {
        // ii caut vecinul in ht_reff
        curr_reff = get_ht_list_reff(data->ht_reff, references[i]);
        // daca nu e adaugat il adaug
        if (curr_reff == NULL) {
            add_ht_list_reff(data->ht_reff, references[i]);
        // daca e, ii cresc numarul de citari
        } else {
            ((article_list_reff *)curr_reff->data)->freq++;
        }
    }
    // adaugare autori in ht_author
    for (i = 0; i < num_authors; i++) {
        add_ht_list_author(data->ht_author, author_ids[i], id, year);
    }
}
char* get_oldest_influence(PublData* data, const int64_t id_paper) {
    int64_t id = id_paper;
    char *name = bfs_list_ref(data->ht_list, data->ht_reff, id);
    // reinitializare visited pentru fiecare articol
    // care a fost vizitat
    clear_visited_node(data->ht_list, id);
    return name;
}

float get_venue_impact_factor(PublData* data, const char* venue) {
    char *venue_new = strdup(venue);
    float res =  get_venue(data->ht_venue, data->ht_reff, venue_new);

    free(venue_new);

    return res;
}

int get_number_of_influenced_papers(PublData* data, const int64_t id_paper,
    const int distance) {
    /* TODO: implement get_number_of_influenced_papers */

    return -1;
}

int get_erdos_distance(PublData* data, const int64_t id1, const int64_t id2) {
    /* TODO: implement get_erdos_distance */

    return -1;
}

char** get_most_cited_papers_by_field(PublData* data, const char* field,
    int* num_papers) {
    /* TODO: implement get_most_cited_papers_by_field */

    return NULL;
}

int get_number_of_papers_between_dates(PublData* data, const int early_date,
    const int late_date) {
    int i;
    int nr = 0;
    // cautare articoe publicate intre anii dati
    for (i = early_date; i <= late_date ; i++) {
        nr += data->freq[i];
    }
    return nr;
}

int get_number_of_authors_with_field(PublData* data, const char* institution,
    const char* field) {

    char *field_new = strdup(field);
    char *institution_new = strdup(institution);

    int64_t *v_authors = calloc(MAX_AUTHORS, sizeof(int64_t));
    int total_auth = get_ht_list_field(data->ht_list, data->ht_field,
                        institution_new, field_new, v_authors);

    free(v_authors);
    free(field_new);
    free(institution_new);

    return total_auth;
}

int* get_histogram_of_citations(PublData* data, const int64_t id_author,
    int* num_years) {

    *num_years = 0;
    int min_year = 2020;
    int64_t aux = id_author;
    int index = hash_function_int(&aux)%N_MAX;
    int *hist = calloc(1, sizeof(int));

    struct Node *curr = data->ht_author->buckets[index].head;
        // parcurgem bucket-ul autorului cu id-ul dat
        while (curr != NULL) {
            if (((article_list_author*)curr->data)->id_author == id_author) {
                int64_t id = ((article_list_author*)curr->data)->id_paper;
                int y = ((article_list_author*)curr->data)->year;
                // daca s-a gasit un articol cu anul aparitiei mai mic
                if (y < min_year) {
                    // marim dimensiunea histogramei
                    hist = realloc(hist, (2020 - y + 1)*sizeof(int));
                    int dif = (2020 - min_year + 1);
                    // completam pozitiile adaugate cu 0
                    memset(hist + dif, 0, (min_year - y)*sizeof(int));
                    // actualizam anul minim
                    min_year = y;
                }
                // cautam articolele scrise de autor in ht_reff
                struct Node *reff = get_ht_list_reff(data->ht_reff, id);
                if (reff != NULL) {
                    // completam histograma cu numarul lor de citari
                    int freq = ((article_reff *)reff->data)->freq;
                    hist[2020-y] += (freq - 1);
                }
            }
            curr = curr->next;
        }
    // calculare num_years;
    *num_years = 2020 - min_year + 1;

    return hist;
}

char** get_reading_order(PublData* data, const int64_t id_paper,
    const int distance, int* num_papers) {
    /* TODO: implement get_reading_order */

    *num_papers = 0;

    return NULL;
}

char* find_best_coordinator(PublData* data, const int64_t id_author) {
    /* TODO: implement find_best_coordinator */

    return NULL;
}
