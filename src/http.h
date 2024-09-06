#ifndef HTTP_H
#define HTTP_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "arena.h"

// No explicit support for any HTTP standard yet
#define HTTP_VERSION "HTTP/1.0"
#define HTTP_RES_MAX_SIZE (1 << 12)

typedef enum {
    HTTP_GET = 1,
    HTTP_POST,
} http_method;

typedef enum {
    HTTP_STATUS_OK = 200,
    HTTP_STATUS_NOT_FOUND = 404,
} http_status;

typedef struct http_header {
    const char *key;
    const char *value;
    struct http_header *next;
} http_header;

typedef struct http_query_param {
    const char *key;
    const char *value;
    struct http_query_param *next;
} http_query_param;

typedef struct {
    http_status status;
    const char *content_type;
    const char *body;
    http_header *headers;
} http_res;

typedef struct {
    http_method method;
    const char *path;
    http_query_param *query_params;
    const char *version;
    http_header *headers;
    const char *body;
} http_req;

http_req http_req_parse(Arena *a, const char *req, int *error);

char *http_res_stringify(Arena *a, http_res res);
char *http_req_stringify(Arena *a, http_req req);

void http_res_file(Arena *a, http_res *res, const char *file_name);
void http_res_not_found(Arena *a, http_res *res);

#endif // HTTP_H
