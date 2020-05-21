/* Copyright 2020 Pasca Mihai; Nicolae Diana */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "hashtable.h"

int compare_function_ints(void *a, void *b) {
    return memcmp(a, b, sizeof(int64_t));
}

int compare_function_strings(void *a, void *b) {
    return strcmp(a, b);
}

unsigned int hash_function_int(void *a) {
    int64_t uint_a = *(int64_t *)a;

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

void init_ht(struct Hashtable *ht, int hmax,
            unsigned int (*hash_function)(void*),
            int (*compare_function)(void*, void*)) {
    int i;
    ht->buckets = malloc(hmax * sizeof(struct LinkedList));
    for (i = 0; i <= hmax - 1; i++) {
        init_list(&(ht -> buckets[i]));
    }
    ht->compare_function = compare_function;
    ht->hash_function = hash_function;
    ht->hmax = hmax;
    ht->size = 0;
}

void put_list(struct Hashtable *ht, void *key, void *value,
             int key_size) {
    unsigned int index = ht->hash_function(key);
    index = index % ht->hmax;
    struct Node *curr;
    struct list_info *entry, *new_list_info;
    curr = ht->buckets[index].head;

    while (curr != NULL) {
        entry = (struct list_info*) curr->data;
        if (!ht->compare_function(key, entry->key)) {
            add_nth_node(entry->list, (1 << 30), value);
            return;
        }
        curr = curr->next;
    }

    new_list_info = malloc(sizeof(struct list_info));
    // Add new key
    new_list_info->key = malloc(key_size);
    memcpy(new_list_info->key, key, key_size);
    // Add new list
    struct LinkedList* new_list;
    new_list = malloc(sizeof(struct LinkedList));
    init_list(new_list);

    new_list_info->list = new_list;
    add_nth_node(&ht->buckets[index], ht->buckets[index].size, new_list_info);
    add_nth_node(new_list_info->list, 0, value);
    ht->size++;
}

void put(struct Hashtable *ht, void *key, void *value, int key_size) {
    unsigned int index = ht->hash_function(key);
    index = index % ht->hmax;
    struct info *new_info;

    new_info = malloc(sizeof(struct info));
    new_info->key = malloc(key_size);
    memcpy(new_info->key, key, key_size);
    new_info->value = value;
    add_nth_node(&ht->buckets[index], ht->buckets[index].size, new_info);
    ht->size++;
}

void* get(struct Hashtable *ht, void *key) {
    unsigned int index = ht->hash_function(key);
    index = index % ht->hmax;

    struct LinkedList bucket = ht->buckets[index];
    struct Node *curr;
    curr = bucket.head;

    while (curr != NULL) {
        struct info *inf = (struct info*)curr->data;
        if (!ht->compare_function(inf->key, key)) {
            return inf->value;
        }
        curr = curr->next;
    }
    return NULL;
}

struct LinkedList* get_list(struct Hashtable *ht, void* key) {
    unsigned int index = ht->hash_function(key);
    index = index % ht->hmax;

    struct Node* curr;
    curr = ht->buckets[index].head;

    while (curr != NULL) {
        struct list_info* entry = curr->data;
        // printf("%s\n", entry->key);
        if (!ht->compare_function(entry->key, key)) {
            return entry->list;
        }
        curr = curr->next;
    }
    return NULL;
}

void free_entry(struct LinkedList *entry) {
    struct Node *curr_node;

    while (entry->head) {
        curr_node = remove_nth_node(entry, 0);
        struct list_info *inf = curr_node->data;
        free(inf->key);
        free_list(&inf->list);
        free(curr_node->data);
        free(curr_node);
    }
}

void free_paper_data(struct paper_data *paper_data) {
    int j;
    free(paper_data->title);
            free(paper_data->venue);

            for (j = 0; j < paper_data->num_authors; j++) {
                free(paper_data->author_names[j]);
                free(paper_data->institutions[j]);
            }

            free(paper_data->author_names);
            free(paper_data->author_ids);
            free(paper_data->institutions);

            for (j = 0; j < paper_data->num_fields; j++) {
                free(paper_data->fields[j]);
            }

            free(paper_data->fields);
            free(paper_data->references);
            free(paper_data);
}

void free_ht(struct Hashtable *ht) {
    int i;
    struct Node *curr_node;
    if (ht == NULL || ht -> buckets == NULL) {
        return;
    }
    for (i = 0; i < ht -> hmax; i++) {
        while (ht -> buckets[i].head != NULL) {
            curr_node = remove_nth_node(&ht->buckets[i], 0);
            struct info *inf = (struct info*)curr_node->data;
            struct paper_data *paper_data = inf->value;
            free_paper_data(paper_data);
            free(inf->key);
            free(curr_node->data);
            free(curr_node);
        }
    }
    free(ht -> buckets);
    ht -> buckets = NULL;
    free(ht);
    ht = NULL;
}

void free_ht_list(struct Hashtable *ht) {
    int i;
    if (ht == NULL || ht -> buckets == NULL) {
        return;
    }
    for (i = 0; i < ht -> hmax; i++) {
        free_entry(&ht->buckets[i]);
    }
    free(ht -> buckets);
    ht -> buckets = NULL;
    free(ht);
    ht = NULL;
}
