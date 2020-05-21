/* Copyright 2020 Pasca Mihai; Nicolae Diana */
#include "publications.h"
#include "Hashtable.h"
#include "Queue.h"

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
    int i;

    // info
    info = malloc(sizeof(struct paper_data));
    DIE(info == NULL, "info malloc");
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
        // Add to the authors hashtable
        put_authors(data, &info->author_ids[i], info, sizeof(int64_t));
    }
    // fields
    info->num_fields = num_fields;
    info->fields = malloc(num_fields * sizeof(char *));
    DIE(info->fields == NULL, "fields malloc");
    for (i = 0; i < num_fields; i++) {
        info->fields[i] = malloc(LMAX * sizeof(char));
        DIE(info->fields[i] == NULL, "fields malloc");
        snprintf(info->fields[i], LMAX, "%s", fields[i]);
        // Add the the field hashtable
        put_field(data, info->fields[i], info, LMAX);
    }
    // references
    info->references = malloc(num_refs * sizeof(int64_t));
    info->num_refs = num_refs;
    DIE(info->references == NULL, "references malloc");
    info->id = id;
    info->verified = 0;

    for (i = 0; i < num_refs; i++) {
        info->references[i] = references[i];
        // Add to the lists of citations
        put_citations(data, &references[i], info, sizeof(int64_t));
    }

    // Add to the id hashtable
    put_id(data, &info->id, info, sizeof(int64_t));
    // Add to the venue hashtable
    put_venue(data, info->venue, info, LMAX);
}

void* get_oldest(PublData*data, const int64_t id_paper, int* ok,
    struct paper_data *oldest, int64_t* verified) {
    struct paper_data *paper_data = get(data, &id_paper, sizeof(int64_t));
    int i, j;

    // The paper was not added
    if (paper_data == NULL)
        return NULL;
    // The paper was already verified
    for (j = 0; j < MAXV; j++) {
        if (verified[j] == id_paper) {
            return NULL;
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
    // Find the oldest citation
    if (oldest->year >= paper_data->year) {
        if (oldest->year == paper_data->year) {
            struct LinkedList* cit_list = get_cit_list(data, &oldest->id);

            oldest->num_cits = cit_list->size;
            cit_list = get_cit_list(data, &paper_data->id);
            paper_data->num_cits = cit_list->size;

            if (oldest->num_cits <= paper_data->num_cits) {
                if (oldest->num_cits == paper_data->num_cits) {
                    if (oldest->id > paper_data->id) {
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
    struct paper_data oldest;
    oldest.year = 2021;
    int64_t *verified;
    int i, ok = 0;

    verified = calloc(MAXV, sizeof(int64_t));
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
    struct LinkedList *venue_list = get_venue_list(data, venue);
    float influence_factor = 0;

    if (!venue_list)
        return 0;

    struct Node *curr = venue_list->head;
    while (curr) {
        struct paper_data *info = curr->data;
        if (!strcmp(info->venue, venue)) {
            struct LinkedList* cit_list = get_cit_list(data, &info->id);
            if (cit_list) {
                influence_factor += cit_list->size;
            }
        }
        curr = curr->next;
    }
    // printf("%lf %d %lf\n", influence_factor, count, influence_factor/count);
    influence_factor /= venue_list->size;
    return influence_factor;
}

void calculate_number(PublData* data, int64_t id_paper, int distance,
    int64_t* verified, int* nr) {
    struct paper_data *paper_data = get(data, &id_paper, sizeof(int64_t));
    int j;

    if (paper_data == NULL)
        return;

    // The paper was already verified
    for (j = 0; j < MAXV; j++) {
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

    struct LinkedList* cit_list = get_cit_list(data, &id_paper);
    if (cit_list) {
        struct Node *curr = cit_list->head;
        while (curr) {
            struct paper_data *data_cit = curr->data;
            calculate_number(data, data_cit->id, distance - 1, verified, nr);
            curr = curr->next;
        }
    }
}

int get_number_of_influenced_papers(PublData* data, const int64_t id_paper,
    const int distance) {
    int nr = 0;
    int64_t *verified;

    verified = calloc(MAXV,  sizeof(int64_t));
    calculate_number(data, id_paper, distance, verified, &nr);
    free(verified);
    return nr - 1;
}

void get_erdos(PublData* data, const int64_t id1, const int64_t id2,
    int64_t* verified, int distance, int* answer) {
    struct LinkedList *author_list = get_authors_list(data, &id1);
    int i, ok = 0;

    if (!author_list)
        return;

    struct Node *curr = author_list->head;
    while (curr) {
        ok = 0;
        struct paper_data *paper = curr->data;
        for (i = 0; i < MAXV; i++) {
            if (verified[i] == paper->id) {
                ok = 1;
                return;
            }
            if (verified[i] == 0) {
                verified[i] = paper->id;
                break;
            }
        }
        for (i = 0; i < paper->num_authors; i++) {
            if (!memcmp(&paper->author_ids[i], &id2, sizeof(int64_t))) {
                if (distance < *answer) {
                    *answer = distance;
                }
                break;
            }
            get_erdos(data, paper->author_ids[i], id2, verified,
                distance + 1, answer);
        }
        curr = curr->next;
    }
}

int get_erdos_distance(PublData* data, const int64_t id1, const int64_t id2) {
    int answer = HMAX;
    int64_t* verified;

    verified = calloc(MAXV,  sizeof(int64_t));
    get_erdos(data, id1, id2, verified, 1, &answer);
    free(verified);

    if (answer != HMAX)
        return answer;
    else
        return -1;
}

void swap(struct paper_data **papers, char **titles, int i, int j) {
    struct paper_data *tmp_paper;
    char *tmp_string;

    tmp_paper = papers[i];
    papers[i] = papers[j];
    papers[j] = tmp_paper;

    // string
    tmp_string = titles[i];
    titles[i] = titles[j];
    titles[j] = tmp_string;
}

char** get_most_cited_papers_by_field(PublData* data, const char* field,
    int* num_papers) {
    struct paper_data **most_cited_paper;
    char **most_cited;
    int *most_cited_count;
    int i = 0, j, number;

    struct LinkedList *field_list = get_field_list(data, field);
    struct Node *curr = field_list->head;

    most_cited_paper = malloc(field_list->size * sizeof(struct paper_data*));
    most_cited = malloc(field_list->size * sizeof(char*));

    while (curr) {
        struct paper_data *paper_data = curr->data;
        struct LinkedList *cits_list = get_cit_list(data, &paper_data->id);

        if (cits_list == NULL) {
            paper_data->num_cits = 0;
        } else {
            paper_data->num_cits = cits_list->size;
        }

        most_cited_paper[i] = paper_data;
        most_cited[i] = malloc(LMAX * sizeof(char));
        snprintf(most_cited[i], LMAX, "%s", paper_data->title);
        i++;
        curr = curr->next;
    }

    number = i;
    if (i < *num_papers) {
        *num_papers = i;
    }

    // printf("%d %d\n", *num_papers, number);
    for (i = 0; i < number; i++) {
        for (j = 0; j < number - i - 1; j++) {
            if (most_cited_paper[j]->num_cits <= most_cited_paper[j + 1]->num_cits) {
                if (most_cited_paper[j]->num_cits == most_cited_paper[j + 1]->num_cits) {
                    if (most_cited_paper[j]->year <= most_cited_paper[j + 1]->year) {
                        if (most_cited_paper[j]->year == most_cited_paper[j + 1]->year) {
                            if (most_cited_paper[j]->id > most_cited_paper[j + 1]->id) {
                                 swap(most_cited_paper, most_cited, j, j + 1);
                            }
                        } else {
                            swap(most_cited_paper, most_cited, j, j + 1);
                        }
                    }
                } else {
                    swap(most_cited_paper, most_cited, j, j+1);
                }
            }
        }
    }

    // printf("new:\n");
    for (i = *num_papers; i < number; i++) {
        free(most_cited[i]);
    }
    free(most_cited_paper);
    return most_cited;
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
    struct LinkedList *field_list = get_field_list(data, field);
    struct Node *curr = field_list->head;
    int x, i, nr = 0;
    int64_t *verified;
    verified = calloc(MAXV,  sizeof(int64_t));

    while (curr) {
        struct paper_data *paper_data = curr->data;
        for (i = 0; i < paper_data->num_authors; i++) {
            if (!strcmp(paper_data->institutions[i], institution)) {
                for (x = 0; x < MAXV; x++) {
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
    curr = curr->next;
    }
    free(verified);
    return nr;
}

int* get_histogram_of_citations(PublData* data, const int64_t id_author,
    int* num_years) {
    struct LinkedList* authors_list = get_authors_list(data, &id_author);
    struct Node *curr = authors_list->head;
    int* histogram;
    int i;

    *num_years = 2021;
    histogram = calloc(YEARS, sizeof(int));

    while (curr) {
        struct paper_data *paper_data = curr->data;
        for (i = 0; i < paper_data->num_authors; i++) {
            if (!memcmp(&paper_data->author_ids[i], &id_author,
                sizeof(int64_t))) {
                struct LinkedList* cit_list = get_cit_list(data,
                    &paper_data->id);
                if (cit_list) {
                    histogram[2020-paper_data->year] += cit_list->size;
                }
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
