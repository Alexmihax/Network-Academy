// Copyright 2020 Pasca Mihai; Nicolae Diana
#include "publications.h"
#include "Hashtable.h"

PublData* init_publ_data(void) {
    PublData *data;
    data = malloc(sizeof(PublData));
    DIE(data == NULL, "data malloc");
    init_ht(data, HMAX, hash_function_int);
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
        struct paper_data *ref = get(data, &references[i]);
        if (ref && ref->verified == 1) {
            ref->num_cits++;
        }
    }
    info->id = id;
    info->verified = 0;
    // info->num_cits = cit_number(data, info);
    // Add to hashtable
    put(data, &info->id, sizeof(int64_t), info);
}

int cit_number(PublData *data, struct paper_data* paper) {
    int a, i, nr_cit = 0;
    for (a = 0; a < HMAX; a++) {
        struct Node *curr = data->buckets[a].head;
        while (curr) {
            struct info *info_cit = curr->data;
            struct paper_data *data_cit = info_cit->value;
            if (paper->year > data_cit->year) {
                curr = curr -> next;
                continue;
            }
            for (i = 0; i < data_cit->num_refs; i++) {
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

void* get_oldest(PublData*data, const int64_t id_paper, int* ok, struct paper_data *oldest, int* verified) {
    int i;
    struct paper_data *paper_data = get(data, &id_paper);
    if (paper_data == NULL)
        return;
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
    for (i = 0; i < paper_data->num_refs; i++) {
        for (int j = 0; j < 10000; j++)
            if (verified[j] == paper_data->references[i]) {
                break;
            }
            else if(verified[j] == 0){
                verified[j] = paper_data->id;
                get_oldest(data, paper_data->references[i], ok, oldest, verified);
                break;
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
    verified = calloc(10000, sizeof(int));
    verified[0] = id_paper;
    struct paper_data *paper_data = get(data, &id_paper);
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

    return 0.f;
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
    /* TODO: implement get_number_of_papers_between_dates */
    int sum = 0, i;
    for (i = early_date - 1950; i <= late_date - 1950; i++) {
        sum += data->year_freq[i];
        //  printf("%d\n", data->year_freq[i]);
    }
    return sum;
}

int get_number_of_authors_with_field(PublData* data, const char* institution,
    const char* field) {
    /* TODO: implement get_number_of_authors_with_field */

    return 0;
}

int* get_histogram_of_citations(PublData* data, const int64_t id_author,
    int* num_years) {
    /* TODO: implement get_histogram_of_citations */

    *num_years = 0;

    return NULL;
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
