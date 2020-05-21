/* Copyright 2020 Pasca Mihai; Nicolae Diana */

#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#define YEARS 80

#include "LinkedList.h"
#include <stdint.h>

struct list_info {
    void* key;
    struct LinkedList* list;
};

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
    struct LinkedList *buckets;
    struct LinkedList *venue;
    struct LinkedList *authors;
    struct LinkedList *field;
    struct LinkedList *citations;
    int size;
    int cit_hmax;
    int hmax; /* Nr. de bucket-uri. */
    unsigned int (*hash_function)(int64_t*);
    int *year_freq;
};

void init_ht(struct Hashtable *ht, int hmax);

void put_id(struct Hashtable *ht, void *key, void *value, int key_size);

void put_venue(struct Hashtable *ht, void *key, void *value, int key_size);

void put_field(struct Hashtable *ht, void *key, void *value, int key_size);

void put_authors(struct Hashtable *ht, void *key, void* value, int key_size);

void put_citations(struct Hashtable *ht, void *key, void *value, int key_size);

void* get(struct Hashtable *ht, const int64_t *key, int key_size);

struct LinkedList* get_cit_list(struct Hashtable *ht, const int64_t* key);

struct LinkedList* get_authors_list(struct Hashtable *ht, const int64_t* key);

struct LinkedList* get_venue_list(struct Hashtable *ht, char* key);

struct LinkedList* get_field_list(struct Hashtable *ht, char* key);

void free_ht(struct Hashtable *ht);

/*
 * Functii de hashing:
 */
unsigned int hash_function_int(int64_t *a);

unsigned int hash_function_string(void *a);

#endif
