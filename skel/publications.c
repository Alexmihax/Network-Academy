/* Copyright 2020 Pasca Mihai; Nicolae Diana */
#include "publications.h"
#include "hashtable.h"
#include "queue.h"
#include "heap.h"

PublData* init_publ_data(void) {
    PublData *data;

    data = malloc(sizeof(PublData));
    DIE(data == NULL, "data malloc");
    data->venue = malloc(sizeof(struct Hashtable));
    DIE(data->venue == NULL, "ht malloc");
    data->authors = malloc(sizeof(struct Hashtable));
    DIE(data->authors == NULL, "ht malloc");
    data->field = malloc(sizeof(struct Hashtable));
    DIE(data->field == NULL, "ht malloc");
    data->citations = malloc(sizeof(struct Hashtable));
    DIE(data->citations == NULL, "ht malloc");
    data->id = malloc(sizeof(struct Hashtable));
    DIE(data->id == NULL, "ht malloc");

    init_ht(data->venue, HMAX, hash_function_string, compare_function_strings);
    init_ht(data->id, HMAX, hash_function_int, compare_function_ints);
    init_ht(data->field, HMAX, hash_function_string, compare_function_strings);
    init_ht(data->authors, HMAX, hash_function_int, compare_function_ints);
    init_ht(data->citations, HMAX, hash_function_int, compare_function_ints);
    data->year_freq = calloc(YEARS, sizeof(int));
    return data;
}

void destroy_publ_data(PublData* data) {
    free_ht(data->id);
    free_ht_list(data->venue);
    free_ht_list(data->field);
    free_ht_list(data->authors);
    free_ht_list(data->citations);
    free(data->year_freq);
    free(data);
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
    data->year_freq[year - 1950]++;
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
        put_list(data->authors, &info->author_ids[i], info, sizeof(int64_t));
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
        put_list(data->field, info->fields[i], info, LMAX);
    }
    // references
    info->references = malloc(num_refs * sizeof(int64_t));
    info->num_refs = num_refs;
    DIE(info->references == NULL, "references malloc");
    info->id = id;

    for (i = 0; i < num_refs; i++) {
        info->references[i] = references[i];
        // Add to the lists of citations
        put_list(data->citations, &info->references[i], info, sizeof(int64_t));
    }

    // Add to the id hashtable
    put(data->id, &info->id, info, sizeof(int64_t));
    // Add to the venue hashtable
    put_list(data->venue, info->venue, info, LMAX);
}

int verified(int64_t *verified, int64_t id) {
    int i;
    for (i = 0; i < MAXV; i++) {
        if (verified[i] == id) {
            return 1;
        }
        if (verified[i] == 0) {
            verified[i] = id;
            return 0;
        }
    }
}

void get_oldest(PublData*data, const int64_t id_paper, int* ok,
    struct paper_data *oldest, int64_t* verified) {
    struct paper_data *paper_data = get(data->id, &id_paper);
    int i, j;

    // The paper was not added
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
    // Search the references of the paper
    for (i = 0; i < paper_data->num_refs; i++) {
        get_oldest(data, paper_data->references[i], ok, oldest, verified);
    }
    // Find the oldest citation
    if (oldest->year >= paper_data->year) {
        if (oldest->year == paper_data->year) {
            struct LinkedList* cit_list = get_list(data->citations,
                                                    &oldest->id);

            oldest->num_cits = cit_list->size;
            cit_list = get_list(data->citations, &paper_data->id);
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
    struct paper_data oldest;
    oldest.year = 2021;
    int64_t *verified;
    int i, ok = 0;

    verified = calloc(MAXV, sizeof(int64_t));
    verified[0] = id_paper;

    struct paper_data *paper_data = get(data->id, &id_paper);
    for (i = 0; i < paper_data->num_refs; i++) {
        get_oldest(data, paper_data->references[i], &ok, &oldest, verified);
    }
    free(verified);

    return ok > 0 ? oldest.title : "None";
}

float get_venue_impact_factor(PublData* data, const char* venue) {
    struct LinkedList *venue_list = get_list(data->venue, venue);
    float influence_factor = 0;

    if (!venue_list) {
        return 0;
    }

    struct Node *curr = venue_list->head;
    while (curr) {
        struct paper_data *info = curr->data;
        if (!strcmp(info->venue, venue)) {
            struct LinkedList* cit_list = get_list(data->citations, &info->id);
            if (cit_list) {
                influence_factor += cit_list->size;
            }
        }
        curr = curr->next;
    }
    influence_factor /= venue_list->size;
    return influence_factor;
}

void calculate_number(PublData* data, int64_t id_paper, int distance,
    int64_t* verified, int* nr) {
    struct paper_data *paper_data = get(data->id, &id_paper);
    int j;

    if (paper_data == NULL) {
        return;
    }

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

    struct LinkedList* cit_list = get_list(data->citations, &id_paper);
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

void free_erdos(struct Hashtable* author, Queue *q) {
    int i;
    struct Node* curr_node;

    // Free hashtable
    for (i = 0; i < author -> hmax; i++) {
        while (author -> buckets[i].head != NULL) {
            curr_node = remove_nth_node(&author->buckets[i], 0);
            struct info *inf = (struct info*)curr_node->data;
            free(inf->value);
            free(inf->key);
            free(curr_node->data);
            free(curr_node);
        }
    }

    free(author -> buckets);
    free(author);
    // Free queue
    purge_q(q);
    free(q);
}

int get_erdos_distance(PublData* data, const int64_t id1, const int64_t id2) {
    Queue *q;
    struct Hashtable* author;
    int *dist, *new_dist, answer, i, j, *id;;

    // initialise hashtable for bfs
    author = malloc(sizeof(struct Hashtable));
    DIE(author == NULL, "author malloc");
    init_ht(author, HMAX, hash_function_int, compare_function_ints);

    // initialise queue
    q = malloc(sizeof(Queue));
    DIE(q == NULL, "queue malloc");
    init_q(q);
    enqueue(q, &id1);

    dist = malloc(sizeof(int));
    DIE(dist == NULL, "distance malloc");
    *dist = 0;
    put(author, &id1, dist, sizeof(int64_t));

    while (!is_empty_q(q)) {
        // Get the first element from the queue
        id = front(q);
        dequeue(q);
        // Get the list of papers for the author
        struct LinkedList *author_list = get_list(data->authors, id);

        if (!author_list) {
            continue;
        }
        dist = get(author, id);
        struct Node *curr = author_list->head;

        while (curr) {
            struct paper_data *paper = curr -> data;
            for (i = 0; i < paper->num_authors; i++) {
                new_dist = get(author, &paper->author_ids[i]);
                if (!new_dist) {
                    if (paper->author_ids[i] == id2) {
                        answer = *dist + 1;
                        free_erdos(author, q);
                        return answer;
                    }
                    new_dist = malloc(sizeof(int));
                    DIE(new_dist == NULL, "new_dist malloc");
                    *new_dist = *dist + 1;
                    put(author, &paper->author_ids[i],
                    new_dist, sizeof(int64_t));
                    enqueue(q, &paper->author_ids[i]);
                }
            }
            curr = curr->next;
        }
    }
    dist = get(author, &id2);
    if (!dist) {
        answer = -1;
    } else {
        answer = *dist;
    }
    free_erdos(author, q);
    return answer;
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
    struct paper_data **paper_list;
    char **most_cited;
    int *most_cited_count;
    int i = 0, j, number;

    struct LinkedList *field_list = get_list(data->field, field);
    struct Node *curr = field_list->head;

    paper_list = malloc(field_list->size * sizeof(struct paper_data*));
    DIE(paper_list == NULL, "paper_list malloc");
    most_cited = malloc(field_list->size * sizeof(char*));
    DIE(most_cited == NULL, "most_cited malloc");

    while (curr) {
        struct paper_data *paper = curr->data;
        struct LinkedList *cits_list = get_list(data->citations, &paper->id);

        if (cits_list == NULL) {
            paper->num_cits = 0;
        } else {
            paper->num_cits = cits_list->size;
        }

        paper_list[i] = paper;
        most_cited[i] = malloc(LMAX * sizeof(char));
        DIE(most_cited[i] == NULL, "most_cited malloc");
        snprintf(most_cited[i], LMAX, "%s", paper->title);
        i++;
        curr = curr->next;
    }

    number = i;
    if (i < *num_papers) {
        *num_papers = i;
    }

    for (i = 0; i < number; i++) {
        for (j = 0; j < number - i - 1; j++) {
            if (paper_list[j]->num_cits <= paper_list[j + 1]->num_cits) {
                if (paper_list[j]->num_cits == paper_list[j + 1]->num_cits) {
                    if (paper_list[j]->year <= paper_list[j + 1]->year) {
                        if (paper_list[j]->year == paper_list[j + 1]->year) {
                            if (paper_list[j]->id > paper_list[j + 1]->id) {
                                 swap(paper_list, most_cited, j, j + 1);
                            }
                        } else {
                            swap(paper_list, most_cited, j, j + 1);
                        }
                    }
                } else {
                    swap(paper_list, most_cited, j, j+1);
                }
            }
        }
    }

    for (i = *num_papers; i < number; i++) {
        free(most_cited[i]);
    }
    free(paper_list);
    return most_cited;
}

int get_number_of_papers_between_dates(PublData* data, const int early_date,
    const int late_date) {
    int sum = 0, i;

    for (i = early_date - 1950; i <= late_date - 1950; i++) {
        sum += data->year_freq[i];
    }
    return sum;
}

int get_number_of_authors_with_field(PublData* data, const char* institution,
    const char* field) {
    struct LinkedList *field_list = get_list(data->field, field);
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
    struct LinkedList* authors_list = get_list(data->authors, &id_author);
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
                struct LinkedList* cit_list = get_list(data->citations,
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

int compare_function(const struct paper_data* paper1,
    const struct paper_data* paper2) {
    if (paper1->dist >= paper2->dist) {
        if (paper1->dist == paper2->dist) {
            if (paper1->year <= paper2->year) {
                if (paper1->year == paper2->year) {
                    if (paper1->id < paper2->id) {
                        return 1;
                    }
                } else return 1;
            } 
        } else return 1;
    }
    return 0;
}
 
char** get_reading_order(PublData* data, const int64_t id_paper,
    const int distance, int* num_papers) {
    heap_t *heap;
    Queue *q;
    struct Hashtable* papers;
    struct paper_data *paper, *new_paper;

    char **answer;
    int64_t *id;
    int i, *dist, *new_dist;

    heap = heap_create(compare_function);
    
    // initialise hashtable for bfs
    papers = malloc(sizeof(struct Hashtable));
    DIE(papers == NULL, "papers malloc");
    init_ht(papers, HMAX, hash_function_int, compare_function_ints);

    // initialise queue
    q = malloc(sizeof(Queue));
    DIE(q == NULL, "queue malloc");
    init_q(q);
    enqueue(q, &id_paper);

    dist = malloc(sizeof(int));
    DIE(dist == NULL, "dist malloc");
    *dist = 0;
    put(papers, &id_paper, dist, sizeof(int64_t));

    while (!is_empty_q(q)) {
        // Get the first element from the queue
        id = front(q);
        dequeue(q);
        // Get the paper
        paper = get(data->id, id);
        if (!paper) {
            continue;
        }
        dist = get(papers, id);
        if (distance == *dist) {
            break;
        }
        for (i = 0; i < paper->num_refs; i++) {
            new_dist = get(papers, &paper->references[i]);
            new_paper = get(data->id, &paper->references[i]);

            if (!new_dist && new_paper) {
                new_dist = malloc(sizeof(int));
                DIE(new_dist == NULL, "new_dist malloc");
                *new_dist = *dist + 1;
                put(papers, &paper->references[i],
                    new_dist, sizeof(int64_t));
                new_paper->dist = *new_dist;
                enqueue(q, &paper->references[i]);
                heap_insert(heap, new_paper);
            }
        }
    }
    *num_papers = heap->size;

    answer = malloc(*num_papers * sizeof(char *));
    DIE(answer == NULL, "answer malloc");
    for (i = 0; i < *num_papers; i++) {
        paper = heap_top(heap);
        heap_pop(heap);
        answer[i] = malloc(LMAX * sizeof(char));
        DIE(answer == NULL, "answer malloc");
        snprintf(answer[i], LMAX, "%s",paper->title);
    }
    free_erdos(papers, q);
    heap_free(heap);
    free(heap);
    return answer;
}

float calculate_score(PublData *data, int64_t id, int e_dist) {
    struct LinkedList *author_list = get_list(data->authors, &id);
    float score = 0, prod;

    if (!author_list) {
        return 0;
    }
    struct Node *curr = author_list->head;

    while (curr) {
        struct paper_data* paper = curr->data;
        struct LinkedList *cit = get_list(data->citations, &paper->id);
        prod = 0;
        if (cit) {
            prod = cit->size;
            prod *= get_venue_impact_factor(data, paper->venue);
        }
        score += prod;
        curr = curr->next;
    }
    score = score / pow(2.718, e_dist);
    return score;
}

char* find_best_coordinator(PublData* data, const int64_t id_author) {
    Queue *q;
    struct Hashtable* author;
    int i, j, *id, min_erd = 6, *dist, *new_dist;
    int64_t min_id = INT64_MAX;
    float score = -1, new_score;
    char *answer = NULL;

    // initialise hashtable for bfs
    author = malloc(sizeof(struct Hashtable));
    DIE(author == NULL, "author malloc");
    init_ht(author, HMAX, hash_function_int, compare_function_ints);

    // initialise queue
    q = malloc(sizeof(Queue));
    DIE(q == NULL, "queue malloc");
    init_q(q);
    enqueue(q, &id_author);

    dist = malloc(sizeof(int));
    DIE(dist == NULL, "dist malloc");
    *dist = 0;
    put(author, &id_author, dist, sizeof(int64_t));

    while (!is_empty_q(q)) {
        // Get the first element from the queue
        id = front(q);
        dequeue(q);
        // Get the list of papers for the author
        struct LinkedList *author_list = get_list(data->authors, id);
        if (!author_list) {
            continue;
        }
        dist = get(author, id);
        struct Node *curr = author_list->head;

        while (curr) {
            struct paper_data *paper = curr -> data;
            for (i = 0; i < paper->num_authors; i++) {
                new_dist = get(author, &paper->author_ids[i]);

                if (!new_dist) {
                    if (*dist == 5) {
                        return answer;
                    }
                    new_dist = malloc(sizeof(int));
                    DIE(new_dist == NULL, "new_dist malloc");
                    *new_dist = *dist + 1;
                    put(author, &paper->author_ids[i],
                    new_dist, sizeof(int64_t));
                    new_score = calculate_score(data, paper->author_ids[i],
                                                 *new_dist);
                    if (score <= new_score) {
                        if(score == new_score) {
                            if (min_erd >= *new_dist) {
                                if (min_erd == *new_dist) {
                                    if(min_id > paper->author_ids[i]) {
                                        min_erd = *new_dist;
                                        min_id = paper->author_ids[i];
                                        answer = paper->author_names[i];
                                        score = new_score;
                                    }
                                }
                                else {
                                    min_erd = *new_dist;
                                    min_id = paper->author_ids[i];
                                    answer = paper->author_names[i];
                                    score = new_score;
                                }
                            }
                        } else {
                            min_erd = *new_dist;
                            min_id = paper->author_ids[i];
                            answer = paper->author_names[i];
                            score = new_score;
                        }
                    }
                    enqueue(q, &paper->author_ids[i]);
                }
            }
            curr = curr->next;
        }
    }
    free_erdos(author, q);
    return answer ? answer : "None";
}
