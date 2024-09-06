#include "http.h"
#include "arena.h"
#include "http_server.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

// TODO:
// ---------------
// Request parsing
// ---------------

static inline int _parse_method(Arena *a, http_req *req, const char *string,
                                size_t *offset) {
    char *tmp = arena_alloc(a, sizeof(char) * (100 + 1));
    size_t i = 0;

    while (true) {
        // Buffers too small for path
        if (*offset >= BUFFER_LEN || i >= 100) {
            fprintf(stderr, "Buffers too small to parse method\n");
            return 1;
        }

        if (string[*offset] == ' ')
            break;
        tmp[i] = string[*offset];
        ++(*offset);
        ++i;
    }

    if (strncmp(tmp, "GET", 3) == 0) {
        req->method = HTTP_GET;
    } else if (strncmp(tmp, "POST", 4) == 0) {
        req->method = HTTP_POST;
    } else {
        fprintf(stderr, "Unknown HTTP method %s\n", tmp);
        return -1;
    }

    ++(*offset);

    return 0;
}

static inline int _parse_path(Arena *a, http_req *req, const char *string,
                              size_t *offset) {
    char *tmp = arena_alloc(a, sizeof(char) * (100 + 1));
    size_t i = 0;

    if (string[*offset] != '/') {
        fprintf(stderr, "Invalid path format\n");
        return 1;
    }

    while (true) {
        // Buffers too small for path
        if (*offset >= BUFFER_LEN || i >= 100) {
            fprintf(stderr, "Buffers too small to parse path\n");
            return 1;
        }

        // Reached end of path
        if (string[*offset] == ' ' || string[*offset] == '?') {
            ++(*offset);
            break;
        }

        tmp[i] = string[*offset];
        ++(*offset);
        ++i;
    }

    req->path = tmp;
    return 0;
}

// static inline int _parse_query_params(Arena *a, http_req *req, const char
// *string,
//                                       size_t *offset) {
//     if (string[*offset] != '?')
//         return 0;

//     ++(*offset);

//     char key_buffer[100 + 1];
//     char value_buffer[100 + 1];

//     size_t i = 0;
//     while (true) {
//         // parse key
//         while (true) {
//             if (*offset > BUFFER_LEN || i >= 100)
//                 return 1;
//             if (string[*offset] == '=') {
//                 // start looking for value
//                 break;
//             }
//             if (string[*offset] == ' ') {
//                 //
//             }
//             key_buffer[i] = string[*offset];
//             ++(*offset);
//             ++i;
//         }

//         // parse value
//         // check if another kv pair exists
//     }
// }

static inline int _parse_version(Arena *a, http_req *req, const char *string,
                                 size_t *offset) {
    char *tmp = arena_alloc(a, sizeof(char) * (100 + 1));
    const char *prefix = "HTTP/";
    size_t i = 0;

    while (true) {
        // Buffers too small for path
        if (*offset >= BUFFER_LEN || i >= 100) {
            fprintf(stderr, "Buffers too small to parse version\n");
            return 1;
        }

        // Check for end delimiter (\r\n)
        if (string[*offset] == '\r') {
            ++(*offset);
            if (*offset <= BUFFER_LEN && string[*offset] == '\n') {
                ++(*offset);
                break;
            }
            return 1;
        }

        if (i <= 4 && string[*offset] != prefix[i]) {
            fprintf(stderr, "HTTP version does not have the prefix 'HTTP/'\n");
            return 1;
        }

        tmp[i] = string[*offset];
        ++(*offset);
        ++i;
    }
    req->version = tmp;
    return 0;
}

static inline int _parse_headers(Arena *a, http_req *req, const char *string,
                                 size_t *offset) {
    // HACK: ignores headers and is unsafe
    while (true) {
        if (string[*offset] == '\r' && string[*offset + 1] == '\n' &&
            string[*offset + 2] == '\r' && string[*offset + 3] == '\n') {
            *offset += 4;
            break;
        }

        ++(*offset);
    }

    return 0;
}

static inline int _parse_body(Arena *a, http_req *req, const char *string,
                              size_t *offset) {
    char *tmp = arena_alloc(a, sizeof(char) * (100 + 1));
    size_t i = 0;

    while (true) {
        // Buffers too small for path
        if (*offset >= BUFFER_LEN || i >= 100) {
            fprintf(stderr, "Buffers too small to parse body\n");
            return 1;
        }

        // Check for end delimiter (\r\n)
        if (string[*offset] == '\0') {
            break;
        }

        tmp[i] = string[*offset];
        ++(*offset);
        ++i;
    }

    req->body = tmp;
    return 0;
}

http_req http_req_parse(Arena *a, const char *req_str, int *error) {
    http_req req = {0};

    char copy[sizeof(char) * (BUFFER_LEN + 1)];
    strncpy(copy, req_str, BUFFER_LEN);
    size_t offset = 0;

    if (_parse_method(a, &req, copy, &offset) != 0) {
        *error = 1;
        return req;
    }

    if (_parse_path(a, &req, copy, &offset) != 0) {
        *error = 1;
        return req;
    }

    // if (_parse_query_params(a, &req, copy, &offset) != 0) {
    //     *error = 1;
    //     return req;
    // }

    if (_parse_version(a, &req, copy, &offset) != 0) {
        *error = 1;
        return req;
    }

    if (_parse_headers(a, &req, copy, &offset) != 0) {
        *error = 1;
        return req;
    }

    if (_parse_body(a, &req, copy, &offset) != 0) {
        *error = 1;
        return req;
    }

    *error = 0;
    return req;
}

// ------------
// Stringifying
// ------------

char *http_res_stringify(Arena *a, http_res res) {
    // TODO: Actually use http headers
    char *out = arena_alloc(a, (1 << 16));

    // TODO: Do without snprintf
    snprintf(out, (1 << 16),
             HTTP_VERSION
             " %d OK\nContent-Type: %s\nContent-Length: %lu\n\r\n\r\n %s\n",
             res.status, res.content_type, strlen(res.body) + 3, res.body);

    return out;
}

char *http_req_stringify(Arena *a, http_req req) {
    // TODO: make this actually work better
    char *dst = arena_alloc(a, sizeof(char) * (1 << 16));

    // TODO: Do without snprintf
    sprintf(dst, "{ method: %s, path: %s, version: %s }",
            req.method == HTTP_GET ? "GET" : "POST", req.path, req.version);
    return dst;
}

// ------------------
// Standard responses
// ------------------

void http_res_file(Arena *a, http_res *res, const char *file_name) {
    FILE *fp = fopen(file_name, "r");

    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s: %s\n", file_name,
                strerror(errno));
        http_res_not_found(a, res);
        return;
    }

    long file_size = -1;
    if (fseek(fp, 0, SEEK_END) != 0 || (file_size = ftell(fp)) < 0 ||
        fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Could not read file %s: %s\n", file_name,
                strerror(errno));
        http_res_not_found(a, res);
        return;
    }

    char *file_content = arena_alloc(a, sizeof(char) * (file_size + 1));

    if (fread(file_content, 1, file_size, fp) != (unsigned long)file_size) {
        fprintf(stderr, "Could not read file %s: %s\n", file_name,
                strerror(errno));
        http_res_not_found(a, res);
        return;
    }

    if (ferror(fp) != 0) {
        http_res_not_found(a, res);
        return;
    }

    if (fclose(fp) != 0) {
        fprintf(stderr, "Could not close file %s: %s\n", file_name,
                strerror(errno));
        http_res_not_found(a, res);
        return;
    }

    res->body = file_content;
    res->status = 200, res->content_type = "text/html";
    return;
}

void http_res_not_found(Arena *a, http_res *res) {
    res->status = HTTP_STATUS_NOT_FOUND;
    res->content_type = "text/plain";
    char *tmp = arena_alloc(a, sizeof(char) * 100);
    strcpy(tmp, "404 Not Found");
    res->body = tmp;
}
