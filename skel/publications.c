#include "publications.h"
#include "Hashtable.h"

PublData* init_publ_data(void) {
    PublData *data;
    data = malloc(sizeof(PublData));
    DIE(data == NULL, "data malloc");
    init_ht(data, HMAX, hash_function_int ,compare_function_ints);
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
    strcpy(info->title, title);
    // info->venue
    info->venue = malloc(LMAX * sizeof(char));
    DIE(info->venue == NULL, "venue malloc");
    strcpy(info->venue, venue);
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
        strcpy(info->author_names[i], author_names[i]);
        // authors_ids
        info->author_ids[i] = author_ids[i];
        // institutions
        info->institutions[i] = malloc(LMAX * sizeof(char));
        DIE(info->institutions[i] == NULL, "institutions malloc");
        strcpy(info->institutions[i], institutions[i]);
    }
    // fields
    info->num_fields = num_fields;
    info->fields = malloc(num_fields * sizeof(char *));
    DIE(info->fields == NULL, "fields malloc");
    for (i = 0; i < num_fields; i++) {
        info->fields[i] = malloc(LMAX * sizeof(char));
        DIE(info->fields[i] == NULL, "fields malloc");
        strcpy(info->fields[i], fields[i]);
    }
    // references
    info->references = malloc(num_refs * sizeof(int64_t));
    info->num_refs = num_refs;
    DIE(info->references == NULL, "references malloc");
    for (i = 0; i < num_refs; i++) {
        info->references[i] = references[i];
    }
    info->id = id;

    // Add to hashtable
    put(data, &info->id, sizeof(int64_t), info);
}

char* get_oldest_influence(PublData* data, const int64_t id_paper) {
    /* TODO: implement get_oldest_influence */

    return NULL;
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
