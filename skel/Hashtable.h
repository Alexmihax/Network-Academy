/* Copyright 2020 Pasca Mihai; Nicolae Diana */

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
    int num_cits;
    int verified;
    int passed;
};

struct Hashtable {
    struct LinkedList *buckets; /* Array de liste simplu-inlantuite. */
    struct LinkedList *venue;
    struct LinkedList *authors;
    struct LinkedList *field;
    // struct LinkedList *venuebuckets;
    int size; /* Nr. total de noduri existente curent in toate bucket-urile. */
    int hmax; /* Nr. de bucket-uri. */
    unsigned int (*hash_function)(int64_t*);
    int *year_freq;
};

void init_ht(struct Hashtable *ht, int hmax);

void put(struct Hashtable *ht, void *key, void *value, int key_size);

void* get(struct Hashtable *ht, const int64_t *key, int key_size);

int has_key(struct Hashtable *ht, void *key);

void remove_ht_entry(struct Hashtable *ht, void *key);

int get_ht_size(struct Hashtable *ht);

int get_ht_hmax(struct Hashtable *ht);

void free_ht(struct Hashtable *ht);

/*
 * Functii de hashing:
 */
unsigned int hash_function_int(int64_t *a);

#endif
