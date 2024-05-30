#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "arena.h"

// No explicit support for any HTTP standard yet
#define HTTP_VERSION "HTTP/1.0"
#define HTTP_RES_BODY_SIZE 1024

typedef enum {
    HTTP_GET,
    HTTP_POST,
    // HTTP_HEAD,
    // HTTP_PUT,
    // HTTP_DELETE,
    // HTTP_CONNECT,
    // HTTP_OPTIONS,
    // HTTP_TRACE,
    // HTTP_PATCH
} http_method;

typedef enum {
    HTTP_STATUS_OK = 200,
    HTTP_STATUS_NOT_FOUND = 404,
} http_status;

typedef struct http_header {
    char *key;
    char *value;
    struct http_header *next;
} http_header;

typedef struct http_query_param {
    char *key;
    char *value;
    struct http_query_param *next;
} http_query_param;

typedef struct {
    http_status status;
    char *content_type;
    char *body;
    http_header *headers;
} http_res;

typedef struct {
    http_method method;
    char *path;
    char *version;
    http_header *headers;
    http_query_param *query_params;
} http_req;

http_req http_req_parse(Arena *a, const char *req);

char *http_res_stringify(Arena *a, http_res res);
char *http_req_stringify(Arena *a, http_req req);

void http_res_file(Arena *a, http_res *res, const char *file_name);
void http_res_not_found(http_res *res);

#endif // HTTP_H
