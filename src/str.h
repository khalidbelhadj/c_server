#ifndef STR_H_
#define STR_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_null.h>

#include "arena.h"

#define str_INIT_CAPACITY 10

typedef struct {
    char *content;
    uint64_t length;
    uint64_t capacity;
} str;

str str_new(Arena *a);
str str_from_cstr(Arena *a, const char *cstr);
str str_copy(Arena *a, str str);
void str_insert(Arena *a, str *str, const char *cstr, uint64_t position);
void str_append(Arena *a, str *str, const char *cstr);
void str_prepend(Arena *a, str *str, const char *cstr);
char str_at(str str, uint64_t position);
bool str_equal(str s1, str s2);

#ifdef STR_IMPLEMENTATION

str str_new(Arena *a) {
    return (str){.content = arena_alloc(a, str_INIT_CAPACITY),
                 .capacity = str_INIT_CAPACITY,
                 .length = 0};
}

str str_from_cstr(Arena *a, const char *cstr) {
    str str = {0};
    str.length = strlen(cstr);
    str.capacity = str.length;
    str.content = arena_alloc(a, str.length);
    strncpy(str.content, cstr, str.length);
    return str;
}

str str_copy(Arena *a, str string) {
    str new_str = (str){
        .length = string.length,
        .capacity = string.capacity,
    };

    new_str.content = arena_alloc(a, string.capacity);
    memcpy(new_str.content, string.content, string.capacity);
    return new_str;
}

static void _str_realloc(Arena *a, str *str) {
    char *new_content = arena_alloc(a, str->capacity * 2);
    memcpy(new_content, str->content, str->length);
    free(str->content);
    str->content = new_content;
    str->capacity = str->capacity * 2;
}

void str_insert(Arena *a, str *string, const char *cstr, uint64_t position) {
    if (string->content == NULL) {
        printf("str content is null\n");
        return;
    }

    uint64_t cstr_length = strlen(cstr);

    if (string->capacity <= string->length + cstr_length + 1) {
        _str_realloc(a, string);
    }

    memcpy(&string->content[position + cstr_length], &string->content[position],
           cstr_length);
    memcpy(&string->content[position], cstr, cstr_length);
    string->length += cstr_length;
}

void str_append(Arena *a, str *str, const char *cstr) {
    str_insert(a, str, cstr, str->length);
}

void str_prepend(Arena *a, str *str, const char *cstr) {
    str_insert(a, str, cstr, 0);
}

char str_at(str str, uint64_t position) {
    if (position < 0 || position >= str.length) {
        printf("index out of range\n");
        exit(1);
    }

    return str.content[position];
}

bool str_equal(str s1, str s2) {
    if (s1.length != s2.length) {
        return false;
    }

    for (uint64_t i = 0; i < s1.length; ++i) {
        if (str_at(s1, i) != str_at(s1, i))
            return false;
    }

    return true;
}

#endif // STR_IMPLEMENTATION
#endif // STR_H_