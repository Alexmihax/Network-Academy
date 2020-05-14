/*
 * Hashtable.h
 * Alexandru-Cosmin Mihai
 */
#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#define YEARS 80

#include "LinkedList.h"
#include <stdint.h>

struct info {
    void *key;
    void *value;
};

struct paper_data {
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
    int num_cit;
};

struct Hashtable {
    struct LinkedList *buckets; /* Array de liste simplu-inlantuite. */
    int size; /* Nr. total de noduri existente curent in toate bucket-urile. */
    int hmax; /* Nr. de bucket-uri. */
    /* (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor. */
    unsigned int (*hash_function)(void*);
    /* (Pointer la) Functie pentru a compara doua chei. */
    int (*compare_function)(void*, void*);
    int *year_freq;
};

void init_ht(struct Hashtable *ht, int hmax, unsigned int (*hash_function)(void*), int (*compare_function)(void*, void*));

void put(struct Hashtable *ht, void *key, int key_size_bytes, void *value);

void* get(struct Hashtable *ht, void *key);

int has_key(struct Hashtable *ht, void *key);

void remove_ht_entry(struct Hashtable *ht, void *key);

int get_ht_size(struct Hashtable *ht);

int get_ht_hmax(struct Hashtable *ht);

void free_ht(struct Hashtable *ht);

/*
 * Functii de comparare a cheilor:
 */
int compare_function_ints(void *a, void *b);

int compare_function_strings(void *a, void *b);

/*
 * Functii de hashing:
 */
unsigned int hash_function_int(void *a);

unsigned int hash_function_string(void *a);

#endif
