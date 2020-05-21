/* Copyright 2020 Pasca Mihai; Nicolae Diana */

#include "heap.h"
#include "hashtable.h"
#include "publications.h"

heap_t *heap_create(int (*cmp_f)(const struct paper_data *,
    const struct paper_data *)) {
    heap_t *heap;

    heap = malloc(sizeof(heap_t));
    DIE(heap == NULL, "heap malloc");

    heap->cmp = cmp_f;
    heap->size = 0;
    heap->capacity = 2;
    heap->arr = malloc(heap->capacity * sizeof(struct paper_data *));
    DIE(heap->arr == NULL, "heap->arr malloc");

    return heap;
}

static void __heap_insert_fix(heap_t *heap, int pos) {
    struct paper_data *tmp_paper;

    if (pos == 0)
        return;

    int p = GO_UP(pos);
    if (heap->cmp(heap->arr[pos], heap->arr[p])) {
        tmp_paper = heap->arr[pos];
        heap->arr[pos] = heap->arr[p];
        heap->arr[p] = tmp_paper;
        __heap_insert_fix(heap, p);
    }
}

void heap_insert(heap_t *heap, struct paper_data *paper) {
    heap->arr[heap->size] = paper;

    __heap_insert_fix(heap, heap->size);

    heap->size++;
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;

        heap->arr = realloc(heap->arr,
        heap->capacity * sizeof(struct paper_data *));
        DIE(heap->arr == NULL, "heap->arr realloc");
    }
}

struct paper_data* heap_top(heap_t *heap) {
    return heap->arr[0];
}

void swap_t(struct paper_data *paper1, struct paper_data *paper2) {
    struct paper_data tmp;
    tmp = *paper1;
    *paper1 = *paper2;
    *paper2 = tmp;
}

static void __heap_pop_fix(heap_t *heap, int pos) {
    int p = pos;
    int l = GO_LEFT(pos);
    int r = GO_RIGHT(pos);
    if (r > heap->size - 2) {
        if (l > heap->size - 2) {
            return;
        }
        if (heap->cmp(heap->arr[l], heap->arr[p])) {
            swap_t(heap->arr[p], heap->arr[l]);
            __heap_pop_fix(heap, l);
        }
    }
    if (heap->cmp(heap->arr[l], heap->arr[r])) {
        if (heap->cmp(heap->arr[l], heap->arr[p])) {
            swap_t(heap->arr[p], heap->arr[l]);
            __heap_pop_fix(heap, l);
        }
    } else if (heap->cmp(heap->arr[l], heap->arr[p])) {
        swap_t(heap->arr[p], heap->arr[r]);
        __heap_pop_fix(heap, r);
    }
}

void heap_pop(heap_t *heap) {
    heap->arr[0] = heap->arr[heap->size-1];

    heap->size--;

    __heap_pop_fix(heap, 0);
}

int heap_empty(heap_t *heap) {
    return heap->size <= 0;
}

void heap_free(heap_t *heap) {
    free(heap->arr);
}
