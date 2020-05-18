// Copyright Pasca Mihai Alexandru 2020
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VHMAX 1000
#define MAXL 200

// StringsHashtable structure with string array and frequency array
typedef struct {
    char *buckets[VHMAX];
    int *count;
    int *nr_cits;
} StringsHashtable;

unsigned int hash_function_string(void *a) {
    /*
     * Credits: http://www.cse.yorku.ca/~oz/hash.html
     */
    unsigned char *puchar_a = (unsigned char*) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

    return hash;
}

int check_ht(StringsHashtable *ht, char *key) {
    // Get the hash for the key
    int index = hash_function_string(key) % VHMAX;
    // Search and compare all the keys in the hashtable
    // increase the frequency if the key is already entered and return true
    for (int i = index; i < VHMAX + index; i++) {
        if (ht->buckets[i%VHMAX] != NULL && !strcmp(key, ht->buckets[i%VHMAX])) {
            ht -> count[i%VHMAX]++;
            return 1;
        }
    }
    // return false if the string was not found
    return 0;
}

void find_new(StringsHashtable *ht, char *key) {
    // Get the hash for the key
    int index = hash_function_string(key) % VHMAX;
    // Search for the next empty bucket and enter the key
    for (int i = index; i < VHMAX + index; i++) {
        if (ht -> buckets[i%VHMAX] == NULL) {
            ht -> buckets[i%VHMAX] = malloc(MAXL * sizeof(char));
            snprintf(ht -> buckets[i%VHMAX], MAXL, "%s", key);
            ht -> count[i%VHMAX]++;
            return;
        }
    }
}

void put_ht_strings(StringsHashtable *ht, char *key) {
    // Check if the key is already entered in the hashtable
    if (check_ht(ht, key)) {
        return;
    }

    // Find the next empty bucket for the key
    find_new(ht, key);
}

void free_hashtable_strings(StringsHashtable *ht) {
    free(ht -> count);
    for (int i = 0; i < VHMAX; i++) {
        free(ht -> buckets[i]);
    }
}

// int main() {
//     StringsHashtable *ht_strings;
//     // String used for reading
//     char str[MAXL];

//     if ((ht_strings = malloc(sizeof(Hashtable))) == NULL) {
//         return 0;
//     }
//     // Initialising the frequency array with 0 in every position
//     // which stores up to 256 values
//     if ((ht_strings -> count = calloc(VHMAX, sizeof(unsigned char))) == NULL) {
//         return 0;
//     }
//     // Initialising every string to point at NULL
//     for (int i = 0; i < VHMAX; i++) {
//         ht_strings -> buckets[i] = NULL;
//     }

//     // Read until the end of the file
//     while (scanf("%s\n", str) != EOF) {
//         put(ht_strings, str);
//     }

//     // Print the strings and the corespondant frequency
//     for (int i = 0; i < VHMAX; i++) {
//         if (ht_strings -> count[i]) {
//             printf("%s %d\n", ht_strings -> buckets[i], ht_strings -> count[i]);
//         }
//     }

//     free_hashtable(ht_strings);
//     free(ht_strings);

//     return 0;
// }
