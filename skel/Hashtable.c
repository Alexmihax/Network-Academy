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

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
void init_ht(struct Hashtable *ht, int hmax) {
    int i;
    ht -> buckets = malloc(hmax * sizeof(struct LinkedList));
    ht -> venue = malloc(hmax * sizeof(struct LinkedList));
    ht -> authors = malloc(hmax * sizeof(struct LinkedList));
    ht -> field = malloc(hmax * sizeof(struct LinkedList));
    for (i = 0; i < hmax; i++) {
        init_list(&(ht -> buckets[i]));
        init_list(&(ht -> venue[i]));
        init_list(&(ht -> authors[i]));
        init_list(&(ht -> field[i]));
    }
    ht -> hmax = hmax;
    ht -> size = 0;
    ht->year_freq = calloc(YEARS, sizeof(int));
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune tipul ei), in momentul in care
 * se creeaza o noua intrare in hashtable (in cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie
 * a valorii la care pointeaza key si adresa acestei copii trebuie salvata in structura info asociata intrarii din ht.
 * Pentru a sti cati octeti trebuie alocati si ;copiati, folositi parametrul key_size_bytes.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel put(ht, key_actual, value_actual),
 * valoarea la care pointeaza key_actual poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia unei intrari din hashtable.
 * Nu ne dorim acest lucru, fiindca exista riscul sa ajungem in situatia in care nu mai stim la ce cheie este
 * inregistrata o anumita valoare.
 */
void put(struct Hashtable *ht, void *key, void *value, int key_size) {
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
    unsigned int index = hash_function_string(key) % ht->hmax;
    struct info *newInfo;
    newInfo = malloc(sizeof(struct info));
    newInfo -> key = malloc(key_size);
    memcpy(newInfo -> key, key, key_size);
    newInfo -> value = value;
    add_nth_node(&ht -> venue[index], ht->venue[index].size, newInfo);
    ht -> size++;
}

void put_field(struct Hashtable *ht, void *key, void *value, int key_size) {
    unsigned int index = hash_function_string(key) % ht->hmax;
    struct info *newInfo;
    newInfo = malloc(sizeof(struct info));
    newInfo -> key = malloc(key_size);
    memcpy(newInfo -> key, key, key_size);
    newInfo -> value = value;
    add_nth_node(&ht -> field[index], ht->field[index].size, newInfo);
    ht -> size++;
}

void put_authors(struct Hashtable *ht, void *key, void *value, int key_size) {
    unsigned int index = hash_function_int(key) % ht->hmax;
    struct info *newInfo;
    newInfo = malloc(sizeof(struct info));
    newInfo -> key = malloc(key_size);
    memcpy(newInfo -> key, key, key_size);
    newInfo -> value = value;
    add_nth_node(&ht -> authors[index], ht->authors[index].size, newInfo);
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

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable folosind functia put
 * 0, altfel.
 */
int has_key(struct Hashtable *ht, void *key) {
    unsigned int index = hash_function_int(key) % ht->hmax;
    struct LinkedList bucket = ht -> buckets[index % ht->hmax];
    struct Node *curr;
    curr = bucket.head;
    while (curr != NULL) {
        struct info *inf = (struct info*)curr -> data;
        if (memcmp(inf -> key, key, sizeof(int64_t)) == 0) {
            return 1;
        }
        curr = curr -> next;
    }
    return 0;
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o intrare din hashtable (adica memoria
 * pentru copia lui key --vezi observatia de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void remove_ht_entry(struct Hashtable *ht, void *key) {
    unsigned int index = hash_function_int(key);
    struct LinkedList bucket = ht -> buckets[index % ht->hmax];
    struct Node *curr;
    int i = 0;
    curr = bucket.head;
    while (curr != NULL) {
        struct info *inf = (struct info*)curr -> data;
        if (memcmp(inf -> key, key, sizeof(int64_t)) == 0) {
            curr = remove_nth_node(&ht -> buckets[index], i);
            free(inf -> key);
            free(curr -> data);
            free(curr);
            ht -> size--;
            return;
        }
        curr = curr -> next;
        i++;
    }
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable, dupa care elibereaza si memoria folosita
 * pentru a stoca structura hashtable.
 */
void free_ht(struct Hashtable *ht) {
    int i, j;
    struct Node *currNode;
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
        while(ht->venue[i].head != NULL) {
            currNode = remove_nth_node(&ht->venue[i], 0);
            struct info *inf = (struct info*)currNode -> data;
            free(inf->key);
            free(currNode->data);
            free(currNode);
        }
        while(ht->authors[i].head != NULL) {
            currNode = remove_nth_node(&ht->authors[i], 0);
            struct info *inf = (struct info*)currNode -> data;
            free(inf->key);
            free(currNode->data);
            free(currNode);
        }
        while(ht->field[i].head != NULL) {
            currNode = remove_nth_node(&ht->field[i], 0);
            struct info *inf = (struct info*)currNode -> data;
            free(inf->key);
            free(currNode->data);
            free(currNode);
        }
    }
    free(ht->field);
    free(ht->authors);
    free(ht -> buckets);
    free(ht->venue);
    ht->buckets = NULL;
    ht->venue = NULL;
    free(ht->year_freq);
    free(ht);
    ht = NULL;
}

int get_ht_size(struct Hashtable *ht) {
    if (ht == NULL) {
        return -1;
    }

    return ht->size;
}

int get_ht_hmax(struct Hashtable *ht) {
    if (ht == NULL) {
        return -1;
    }

    return ht->hmax;
}
