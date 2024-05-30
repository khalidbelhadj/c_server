#ifndef ARENA_H_
#define ARENA_H_

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define REGION_CAPACITY ((uint64_t)1 << 16)
#define MAX(a, b) (a > b ? a : b)

typedef struct Region {
    struct Region *next;
    uint64_t capacity;
    uint64_t size;
    void *data[];
} Region;

Region *region_new(uint64_t capacity);
void region_free(Region *r);

typedef struct {
    Region *first;
    Region *last;
} Arena;

void *arena_alloc(Arena *a, uint64_t size);
void arena_free(Arena *a);

#ifdef ARENA_IMPLEMEMTATION

Region *region_new(uint64_t capacity) {
    Region *r = calloc(1, sizeof(Region) + sizeof(void *) * capacity);
    *r = (Region){
        .next = NULL,
        .capacity = capacity,
        .size = 0,
    };
    return r;
}

void region_free(Region *r) { free(r); }

void *arena_alloc(Arena *a, uint64_t size) {
    if (a->last == NULL) {
        // No regions yet
        assert(a->first == NULL);
        a->last = region_new(size > REGION_CAPACITY ? size : REGION_CAPACITY);
        a->first = a->last;
    }

    if (a->last->capacity < a->last->size + size) {
        // Not enough space in a->end
        a->last->next =
            region_new(size > REGION_CAPACITY ? size : REGION_CAPACITY);
        a->last = a->last->next;
    }

    void *res = &a->last->data[a->last->size];
    a->last->size += size;

    return res;
}

void arena_free(Arena *a) {
    Region *current = a->first;
    while (current != NULL) {
        Region *tmp = current->next;
        region_free(current);
        current = tmp;
    }
    *a = (Arena){0};
}

#endif // ARENA_IMPLEMEMTATION

#endif // ARENA_H_
