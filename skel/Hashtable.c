/* Copyright 2020 Pasca Mihai; Nicolae Diana */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Hashtable.h"

/*
 * Functii de hashing:
 */
unsigned int hash_function_int(int64_t *a) {
    int64_t uint_a = *a;

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
    unsigned long hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

    return hash;
}

void init_ht(struct Hashtable *ht, int hmax) {
    int i;
    ht -> buckets = malloc(hmax * sizeof(struct LinkedList));
    ht -> venue = malloc(hmax * sizeof(struct LinkedList));
    ht -> authors = malloc(hmax * sizeof(struct LinkedList));
    ht -> field = malloc(hmax * sizeof(struct LinkedList));
    ht -> citations = malloc(hmax * sizeof(struct LinkedList));
    for (i = 0; i < hmax; i++) {
        init_list(&(ht -> buckets[i]));
        init_list(&(ht -> venue[i]));
        init_list(&(ht -> authors[i]));
        init_list(&(ht -> field[i]));
        init_list(&(ht -> citations[i]));
    }
    ht -> hmax = hmax;
    ht -> size = 0;
    ht->year_freq = calloc(YEARS, sizeof(int));
}

void put_id(struct Hashtable *ht, void *key, void *value, int key_size) {
    unsigned int index = hash_function_int(key) % ht->hmax;
    struct info *newInfo;
    newInfo = malloc(sizeof(struct info));
    newInfo -> key = malloc(key_size);
    memcpy(newInfo -> key, key, key_size);
    newInfo -> value = value;
    add_nth_node(&ht -> buckets[index], ht->buckets[index].size, newInfo);
    ht -> size++;
}

void put_venue(struct Hashtable *ht, void *key, void *value, int key_size) {
    unsigned int index = hash_function_string(key);
    index = index % ht -> hmax;
    struct Node *curr;
    struct list_info *entry, *newVenue;
    curr = ht->venue[index].head;
    while (curr != NULL) {
        entry = (struct list_info*)curr -> data;
        if (!strcmp(entry -> key, key)) {
            add_nth_node(entry->list, 1000, value);
            return;
        }
        curr = curr -> next;
    }
    newVenue = malloc(sizeof(struct list_info));
    // Add new key
    newVenue -> key = malloc(key_size * sizeof(char));
    memcpy(newVenue -> key, key, key_size * sizeof(char));
    // Add new list
    struct LinkedList* newList;
    newList = malloc(sizeof(struct LinkedList));
    init_list(newList);
    newVenue->list = newList;
    add_nth_node(&ht -> venue[index], ht->venue[index].size, newVenue);
    add_nth_node(newVenue->list, 0, value);
    ht -> size++;
}

void put_field(struct Hashtable *ht, void *key, void *value, int key_size) {
    unsigned int index = hash_function_string(key);
    index = index % ht -> hmax;
    struct Node *curr;
    struct list_info *entry, *newField;
    curr = ht->field[index].head;
    while (curr != NULL) {
        entry = (struct list_info*)curr -> data;
        if (!strcmp(entry -> key, key)) {
            add_nth_node(entry->list, 1000, value);
            return;
        }
        curr = curr -> next;
    }
    newField = malloc(sizeof(struct list_info));
    // Add new key
    newField -> key = malloc(key_size * sizeof(char));
    memcpy(newField -> key, key, key_size * sizeof(char));
    // Add new list
    struct LinkedList* newList;
    newList = malloc(sizeof(struct LinkedList));
    init_list(newList);
    newField->list = newList;
    add_nth_node(&ht->field[index], ht->field[index].size, newField);
    add_nth_node(newField->list, 0, value);
    ht -> size++;
}

void put_authors(struct Hashtable *ht, void *key, void* value, int key_size) {
    unsigned int index = hash_function_int(key);
    index = index % ht -> hmax;
    int i;
    struct Node *curr;
    struct list_info *entry, *newAuthor;
    curr = ht->authors[index].head;
    while (curr != NULL) {
        entry = (struct list_info*)curr -> data;
        if (memcmp(entry->key, key, key_size) == 0) {
            add_nth_node(entry->list, (1 << 30), value);
            return;
        }
        curr = curr -> next;
    }
    newAuthor = malloc(sizeof(struct list_info));
    // Add new key
    newAuthor -> key = malloc(key_size);
    memcpy(newAuthor -> key, key, key_size);
    // Add new list
    struct LinkedList* newList;
    newList = malloc(sizeof(struct LinkedList));
    init_list(newList);
    newAuthor->list = newList;
    add_nth_node(&ht->authors[index], ht->authors[index].size, newAuthor);
    add_nth_node(newAuthor->list, 0, value);
    ht -> size++;
}

void put_citations(struct Hashtable *ht, void *key, void *value, int key_size) {
    unsigned int index = hash_function_int(key);
    index = index % ht -> hmax;
    struct Node *curr;
    struct list_info *cit, *newCit;
    curr = ht -> citations[index].head;
    while (curr != NULL) {
        cit = (struct list_info*)curr -> data;
        if (memcmp(cit -> key, key, key_size) == 0) {
            add_nth_node(cit->list, 1000, value);
            return;
        }
        curr = curr -> next;
    }
    newCit = malloc(sizeof(struct list_info));
    // Add new key
    newCit -> key = malloc(key_size);
    memcpy(newCit -> key, key, key_size);
    // Add new list
    struct LinkedList* newList;
    newList = malloc(sizeof(struct LinkedList));
    init_list(newList);
    newCit->list = newList;
    add_nth_node(&ht -> citations[index], ht->citations[index].size, newCit);
    add_nth_node(newCit->list, 0, value);
    ht -> size++;
}


void* get(struct Hashtable *ht, const int64_t *key, int key_size) {
    unsigned int index = hash_function_int(key);
    index = index % ht->hmax;
    struct LinkedList bucket = ht -> buckets[index];
    struct Node *curr;
    curr = bucket.head;
    while (curr != NULL) {
        struct info *inf = (struct info*)curr -> data;
        if (memcmp(inf -> key, key, key_size) == 0) {
            return inf -> value;
        }
        curr = curr -> next;
    }
    return NULL;
}

struct LinkedList* get_cit_list(struct Hashtable *ht, const int64_t* key) {
    unsigned int index = hash_function_int(key);
    index = index % ht->hmax;
    struct Node* curr;
    curr = ht->citations[index].head;
    while (curr != NULL) {
        struct list_info* cit = curr -> data;
        if (memcmp(cit -> key, key, sizeof(int64_t)) == 0) {
            return cit->list;
        }
        curr = curr -> next;
    }
    return NULL;
}

struct LinkedList* get_authors_list(struct Hashtable *ht, const int64_t* key) {
    unsigned int index = hash_function_int(key);
    index = index % ht->hmax;
    struct Node* curr;
    curr = ht->authors[index].head;
    while (curr != NULL) {
        struct list_info* inf = curr -> data;
        if (!memcmp(inf->key, key, sizeof(int64_t))) {
            return inf->list;
        }
        curr = curr -> next;
    }
    return NULL;
}

struct LinkedList* get_venue_list(struct Hashtable *ht, char* key) {
    unsigned int index = hash_function_string(key);
    index = index % ht->hmax;
    struct Node* curr;
    curr = ht->venue[index].head;
    while (curr != NULL) {
        struct list_info* entry = curr -> data;
        if (!strcmp(entry -> key, key)) {
            return entry->list;
        }
        curr = curr -> next;
    }
    return NULL;
}

struct LinkedList* get_field_list(struct Hashtable *ht, char* key) {
    unsigned int index = hash_function_string(key);
    index = index % ht->hmax;
    struct Node* curr;
    curr = ht->field[index].head;
    while (curr != NULL) {
        struct list_info* entry = curr -> data;
        if (!strcmp(entry -> key, key)) {
            return entry->list;
        }
        curr = curr -> next;
    }
    return NULL;
}

void free_ht(struct Hashtable *ht) {
    int i, j;
    struct Node *currNode, *currNode2;
    if (ht == NULL || ht -> buckets == NULL) {
        return;
    }
    for (i = 0; i < ht->hmax; i++) {
        while (ht -> buckets[i].head != NULL) {
            currNode = remove_nth_node(&ht -> buckets[i], 0);
            struct info *inf = (struct info*)currNode -> data;
            struct paper_data *paper_data = inf->value;
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
            free(inf -> key);
            free(currNode -> data);
            free(currNode);
        }
        while (ht->venue[i].head != NULL) {
            currNode = remove_nth_node(&ht->venue[i], 0);
            struct list_info *inf = currNode -> data;
            free(inf->key);
            free_list(&inf->list);
            free(currNode->data);
            free(currNode);
        }
        while (ht->authors[i].head != NULL) {
            currNode = remove_nth_node(&ht->authors[i], 0);
            struct list_info *inf = currNode -> data;
            free(inf->key);
            free_list(&inf->list);
            free(currNode->data);
            free(currNode);
        }
        while (ht->field[i].head != NULL) {
            currNode = remove_nth_node(&ht->field[i], 0);
            struct list_info *inf = currNode -> data;
            free(inf->key);
            free_list(&inf->list);
            free(currNode->data);
            free(currNode);
        }
        while (ht->citations[i].head != NULL) {
            currNode = remove_nth_node(&ht->citations[i], 0);
            struct list_info *cit = currNode -> data;
            free(cit->key);
            free_list(&cit->list);
            free(currNode->data);
            free(currNode);
        }
    }
    free(ht -> citations);
    free(ht->field);
    free(ht->authors);
    free(ht -> buckets);
    free(ht->venue);
    ht->buckets = NULL;
    ht->venue = NULL;
    ht->authors = NULL;
    ht->field = NULL;
    free(ht->year_freq);
    free(ht);
    ht = NULL;
}
