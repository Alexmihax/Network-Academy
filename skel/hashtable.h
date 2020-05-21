/* Copyright 2020 Pasca Mihai; Nicolae Diana */

#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#define YEARS 80
#define LMAX 300

#include "linkedList.h"
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
    int dist;
};

struct PublData {
    struct Hashtable *id;
    struct Hashtable *venue;
    struct Hashtable *authors;
    struct Hashtable *field;
    struct Hashtable *citations;
    int *year_freq;
};

struct Hashtable {
    struct LinkedList *buckets;
    int size;
    int hmax;
    unsigned int (*hash_function)(void*);
    int (*compare_function)(void*, void*);
};

void init_ht(struct Hashtable *ht, int hmax,
            unsigned int (*hash_function)(void*),
            int (*compare_function)(void*, void*));

void put_list(struct Hashtable *ht, void *key, void *value, int key_size);

void put(struct Hashtable *ht, void *key, void *value, int key_size);

void* get(struct Hashtable *ht, void *key);

struct LinkedList* get_list(struct Hashtable *ht, void* key);

void free_paper_data(struct paper_data *paper_data);

void free_entry(struct LinkedList *entry);

void free_ht(struct Hashtable *ht);

void free_ht_list(struct Hashtable *ht);

unsigned int hash_function_int(void *a);

unsigned int hash_function_string(void *a);

int compare_function_ints(void *a, void *b);

int compare_function_strings(void *a, void *b);

#endif
