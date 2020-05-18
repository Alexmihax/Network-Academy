/* Copyright 2020 Pasca Mihai; Nicolae Diana */
#include "publications.h"
#include "Hashtable.h"
// #include "hash.c"

PublData* init_publ_data(void) {
    PublData *data;
    data = malloc(sizeof(PublData));
    DIE(data == NULL, "data malloc");
    init_ht(data, HMAX);
    return data;
}

void destroy_publ_data(PublData* data) {
    free_ht(data);
}

void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, const int64_t id, const int64_t* references,
    const int num_refs) {
    struct paper_data *info;
    // info
    info = malloc(sizeof(struct paper_data));
    DIE(info == NULL, "info malloc");
    int i;
    // info->title
    info->title = malloc(LMAX * sizeof(char));
    DIE(info->title == NULL, "title malloc");
    snprintf(info->title, LMAX, "%s", title);
    // info->venue
    info->venue = malloc(LMAX * sizeof(char));
    DIE(info->venue == NULL, "venue malloc");
    snprintf(info->venue, LMAX, "%s", venue);
    // year
    info->year = year;
    info->num_authors = num_authors;
    data->year_freq[year-1950]++;
    // author_names
    info->author_names = malloc(num_authors * sizeof(char *));
    DIE(info->author_names == NULL, "authors malloc");
    // author_ids
    info->author_ids = malloc(num_authors * sizeof(int64_t));
    DIE(info->author_ids == NULL, "author id malloc");
    // institution
    info->institutions = malloc(num_authors * sizeof(char *));
    DIE(info->institutions == NULL, "institutions malloc");
    for (i = 0; i < num_authors; i++) {
        // author names
        info->author_names[i] = malloc(LMAX * sizeof(char));
        DIE(info->author_names[i] == NULL, "authors_names malloc");
        snprintf(info->author_names[i], LMAX, "%s", author_names[i]);
        // authors_ids
        info->author_ids[i] = author_ids[i];
        // institutions
        info->institutions[i] = malloc(LMAX * sizeof(char));
        DIE(info->institutions[i] == NULL, "institutions malloc");
        snprintf(info->institutions[i], LMAX, "%s", institutions[i]);
    }
    // fields
    info->num_fields = num_fields;
    info->fields = malloc(num_fields * sizeof(char *));
    DIE(info->fields == NULL, "fields malloc");
    for (i = 0; i < num_fields; i++) {
        info->fields[i] = malloc(LMAX * sizeof(char));
        DIE(info->fields[i] == NULL, "fields malloc");
        snprintf(info->fields[i], LMAX, "%s", fields[i]);
    }
    // references
    info->references = malloc(num_refs * sizeof(int64_t));
    info->num_refs = num_refs;
    DIE(info->references == NULL, "references malloc");
    for (i = 0; i < num_refs; i++) {
        info->references[i] = references[i];
        struct paper_data *ref = get(data, &references[i], sizeof(int64_t));
        if (ref && ref->verified == 1) {
            ref->num_cits++;
        }
    }
    info->id = id;
    // info->num_cits = cit_number(data, info);
    // put_ht_strings(data, info->venue);
    info->verified = 0;
    // Add to hashtable
    put(data, &info->id, info, sizeof(int64_t));
    put_venue(data, info->venue, info, LMAX);
    for(i = 0; i < num_authors; i++) {
        put_authors(data, &info->author_ids[i], info, sizeof(int64_t));
    }
    for(i = 0; i < num_fields; i++) {
        put_field(data, info->fields[i], info, LMAX);
    }
}

int cit_number(PublData *data, struct paper_data* paper) {
    int a, i, nr_cit = 0;
    for (a = 0; a < HMAX; a++) {
        struct Node *curr = data->buckets[a].head;
        while (curr) {
            struct info *info_cit = curr->data;
            struct paper_data *data_cit = info_cit->value;
            for (i = 0; i < data_cit->num_refs; i++) {
                if (data_cit->references[i] > paper->id)
                    break;
                if (data_cit->references[i] == paper->id){
                    nr_cit++;
                    break;
                }
            }
            curr = curr->next;
        }
    }
    return nr_cit;
}

void* get_oldest(PublData*data, const int64_t id_paper, int* ok, struct paper_data *oldest, int64_t* verified) {
    int i, j;
    struct paper_data *paper_data = get(data, &id_paper, sizeof(int64_t));
    // The paper was not added
    if (paper_data == NULL)
        return;
    // The paper was already verified
    for (j = 0; j < 10000; j++) {
        if (verified[j] == id_paper) {
            return;
        }
        if (verified[j] == 0) {
            verified[j] = paper_data->id;
            break;
        }
    }
    // Search the references of the paper
    for (i = 0; i < paper_data->num_refs; i++) {
        get_oldest(data, paper_data->references[i], ok, oldest, verified);
    }
    // Find the oldest reference
    if (oldest->year >= paper_data->year) {
        if (oldest->year == paper_data->year) {
            if (!oldest->verified) {
                oldest->num_cits = cit_number(data, oldest);
                oldest->verified = 1;
            }
            if(!paper_data->verified) {
                paper_data->num_cits = cit_number(data, paper_data);
                paper_data->verified = 1;
            }
            if(oldest->num_cits <= paper_data->num_cits) { 
                if (oldest->num_cits == paper_data->num_cits) {
                    if(oldest->id > paper_data->id) {
                        *ok = *ok + 1;
                        *oldest = *paper_data;
                    }
                } else {
                *ok = *ok + 1;
                *oldest = *paper_data;
                }
            }
        } else {
            *ok = *ok + 1;
            *oldest = *paper_data;
        }
    }
}

char* get_oldest_influence(PublData* data, const int64_t id_paper) {
    /* TODO: implement get_oldest_influence */
    int i;
    int ok = 0;
    struct paper_data oldest;
    oldest.year = 2021;
    int64_t *verified;
    verified = calloc(10000, sizeof(int64_t));
    verified[0] = id_paper;
    struct paper_data *paper_data = get(data, &id_paper, sizeof(int64_t));
    for (i = 0; i < paper_data->num_refs; i++) {
        get_oldest(data, paper_data->references[i], &ok, &oldest, verified);
    }
    free(verified);
    if (ok > 0)
        return oldest.title;
    else
        return "None";
}

float get_venue_impact_factor(PublData* data, const char* venue) {
    /* TODO: implement get_venue_impact_factor */
    float influence_factor = 0;
    int a, i, count = 0;
    int index = hash_function_string(venue) % HMAX;
    struct LinkedList venue_list = data -> venue[index];
    struct Node *curr = venue_list.head;
    while (curr) {
        struct info *info = curr->data;
        if (!strcmp(info->key, venue)) {
        struct paper_data *paper = info->value;
            if (!paper->verified) {
                paper->num_cits = cit_number(data, paper);
                paper->verified = 1;
            }
            influence_factor += paper->num_cits;
            count++;
        }
        curr = curr -> next;
    }
    // printf("%lf %d %lf\n", influence_factor, count, influence_factor/count);
    influence_factor /= count;
    return influence_factor;
}

void calculate_number(PublData* data, int64_t id_paper, int distance, int64_t* verified, int* nr) {
    struct paper_data *paper_data = get(data, &id_paper, sizeof(int64_t));
    int a, i, j;
    if (paper_data == NULL)
        return;
    // printf("%lld\n", id_paper);
    // The paper was already verified
    for (j = 0; j < 10000; j++) {
        if (verified[j] == id_paper) {
            return;
        }
        if (verified[j] == 0) {
            verified[j] = paper_data->id;
            break;
        }
    }
    *nr = *nr + 1;
    // Reached the maximum distance
    if (distance == 0) {
        return;
    }
    for (a = 0; a < HMAX; a++) {
        struct Node *curr = data->buckets[a].head;
        while (curr) {
            struct info *info_cit = curr->data;
            struct paper_data *data_cit = info_cit->value;
            for (i = 0; i < data_cit->num_refs; i++) {
                if (data_cit->references[i] > id_paper)
                    break;
                if (data_cit->references[i] == id_paper){
                    calculate_number(data, data_cit->id, distance - 1, verified, nr);
                    break;
                }
            }
            curr = curr->next;
        }
    }
}

int get_number_of_influenced_papers(PublData* data, const int64_t id_paper,
    const int distance) {
    /* TODO: implement get_number_of_influenced_papers */
    int nr = 0, i;
    int64_t *verified;
    verified = calloc(10000,  sizeof(int64_t));
    // printf("start:");
    calculate_number(data, id_paper, distance, verified, &nr);
    free(verified);
    return nr - 1;
}

int get_erdos_distance(PublData* data, const int64_t id1, const int64_t id2) {
    /* TODO: implement get_erdos_distance */

    return -1;
}

char** get_most_cited_papers_by_field(PublData* data, const char* field,
    int* num_papers) {
    unsigned int index = hash_function_string(field);
    index = index % HMAX;

    struct LinkedList field_list = data->field[index];
    struct Node *curr = field_list.head;

    return NULL;
}

int get_number_of_papers_between_dates(PublData* data, const int early_date,
    const int late_date) {
    /* TODO: implement get_number_of_papers_between_dates */
    int sum = 0, i;
    for (i = early_date - 1950; i <= late_date - 1950; i++) {
        sum += data->year_freq[i];
    }
    return sum;
}

int get_number_of_authors_with_field(PublData* data, const char* institution,
    const char* field) {
    /* TODO: implement get_number_of_authors_with_field */
    int a, x, i, j, nr = 0;
    unsigned int index = hash_function_string(field);
    index = index % HMAX;
    int64_t *verified;
    verified = calloc(10000,  sizeof(int64_t));
    struct LinkedList field_list = data->field[index];
    struct Node *curr = field_list.head;
    while (curr) {
        struct info *info = curr->data;
        struct paper_data *paper_data = info->value;
        if (!strcmp(info->key, field)) {
            for (i = 0; i < paper_data->num_authors; i++) {
                if (!strcmp(paper_data->institutions[i], institution)) {
                    for (x = 0; x < 10000; x++) {
                        if (verified[x] == paper_data->author_ids[i]) {
                            break;
                        }
                        if (verified[x] == 0) {
                            nr++;
                            verified[x] = paper_data->author_ids[i];
                            break;
                        }
                    }
                }
            }
        }
    curr = curr->next;
    }
    free(verified);
    return nr;
}

void cit_number_years(PublData *data, struct paper_data* paper, int* histogram) {
    int a, i, nr_cit = 0;
    for (a = 0; a < HMAX; a++) {
        struct Node *curr = data->buckets[a].head;
        while (curr) {
            struct info *info_cit = curr->data;
            struct paper_data *data_cit = info_cit->value;
            for (i = 0; i < data_cit->num_refs; i++) {
                if (data_cit->references[i] > paper->id)
                    break;
                if (data_cit->references[i] == paper->id){
                    histogram[2020-paper->year] += 1;
                    break;
                }
            }
            curr = curr->next;
        }
    }
}

int* get_histogram_of_citations(PublData* data, const int64_t id_author,
    int* num_years) {
    /* TODO: implement get_histogram_of_citations */
    int i, a;
    int index = hash_function_int(&id_author) % data->hmax;
    int* histogram;
    *num_years = 2021;
    histogram = calloc(YEARS, sizeof(int));
    struct Node *curr = data->authors[index].head;
    while (curr) {
        struct info *info = curr->data;
        struct paper_data *paper_data = info->value;
        for (i = 0; i < paper_data->num_authors; i++) {
            if (!memcmp(&paper_data->author_ids[i], &id_author, sizeof(int64_t))){
                cit_number_years(data, paper_data, histogram);
                if (*num_years > paper_data->year) {
                    *num_years = paper_data->year;
                }
                break;
            }
        }
        curr = curr->next;
    }
    *num_years = 2021 - *num_years; 
    return histogram;
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
