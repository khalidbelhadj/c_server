#include "http.h"
#include "http_server.h"

#include <stdio.h>
#include <string.h>

char *http_res_stringify(Arena *a, http_res res) {
    // TODO: Actually use http headersr
    char *dst = arena_alloc(a, sizeof(char) * 10000);
    snprintf(dst, 1000,
             HTTP_VERSION
             " %d OK\nContent-Type: %s\nContent-Length: %lu\n\r\n\r\n %s\n",
             res.status, res.content_type, strlen(res.body) + 4, res.body);
    return dst;
}

static inline void _parse_method(Arena *a, http_req *req, char *str) {
    unsigned int i = 0;
    char *scratch = arena_alloc(a, sizeof(char) * 512 + 1);
    while (*str != ' ') {
        scratch[i] = str[i];
        ++i;

        if (i >= 512) {
            fprintf(stderr, "Method too long\n");
            exit(1);
        }
    }

    if (strcmp(scratch, "GET") == 0) {
        req->method = HTTP_GET;
    } else if (strcmp(scratch, "POST") == 0) {
        req->method = HTTP_POST;
    } else {
        fprintf(stderr, "Unknown method in request: %s\n", scratch);
        exit(1);
    }

    str = &str[i];
}

static inline void _parse_path(Arena *a, http_req *req, char *str) {
    req->path = arena_alloc(a, sizeof(char) * 2083);
    unsigned int i = 0;
    while (str[i] != ' ' && str[i] != '?') {
        req->path[i] = str[i];
        ++i;

        if (i >= 2083) {
            fprintf(stderr, "Request path exceeds 2083 characters\n");
            exit(1);
        }
    }
    str = &str[i];
}

static inline void _parse_query_params(Arena *a, http_req *req, cha *str) {
    unsigned int i = 0;
    if (str[i] == '?') {
        // We have query params
        ++i;
    }

    str = &str[i];
}

static inline void _parse_version(Arena *a, http_req *req, char *str);
static inline void _parse_headers(Arena *a, http_req *req, char *str);
static inline void _parse_body(Arena *a, http_req *req, char *str);

http_req http_req_parse(Arena *a, const char *req_str) {
    http_req req = {0};
    unsigned int i;

    char copy[BUFFER_LEN + 1];
    strncpy(copy, req_str, BUFFER_LEN);

    _parse_method(a, &req, copy);
    _parse_path(a, &req, copy);
    _parse_query_params(a, &req, copy);
    _parse_version(a, &req, copy);
    _parse_headers(a, &req, copy);
    _parse_body(a, &req, copy);

    return req;
}

char *http_req_stringify(Arena *a, http_req req) {
    // TODO: make this actually work better
    char *dst = arena_alloc(a, sizeof(char) * 10000);
    sprintf(dst, "{ method: %s, path: %s, version: %s }",
            req.method == HTTP_GET ? "GET" : "POST", req.path, req.version);
    return dst;
}

void http_res_file(Arena *a, http_res *res, const char *file_name) {
    FILE *fp = fopen(file_name, "r");

    if (fp == NULL)
        goto error;

    if (fseek(fp, 0, SEEK_END) != 0)
        goto error;

    long file_size = ftell(fp);

    if (file_size < 0)
        goto error;

    if (fseek(fp, 0, SEEK_SET) != 0)
        goto error;

    res->body = arena_alloc(a, sizeof(char) * (file_size + 1));

    if (fread(res->body, 1, file_size, fp) != (unsigned long)file_size)
        goto error;

    if (ferror(fp) != 0)
        goto error;

    if (fclose(fp) != 0)
        goto error;

    res->status = 200, res->content_type = "text/html";
    return;

error:
    http_res_not_found(res);
}

void http_res_not_found(http_res *res) {
    res->status = HTTP_STATUS_NOT_FOUND;
    res->content_type = "text/plain";
    res->body = calloc(1, sizeof(char) * 100);
    strcpy(res->body, "404 Not Found");
}
