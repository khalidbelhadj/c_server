#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http.h"

#define BUFFER_LEN (1 << 11)

typedef struct route {
    const char *path;
    http_method method;
    void (*handler)(Arena *, http_req, http_res *);
    struct route *next;
} route;

typedef struct {
    size_t port;
    size_t server_fd;
    route *routes;
    Arena _arena;
} http_server;

typedef struct {
    int client_fd;
    http_server server;
} thread_args;

http_server http_server_new(size_t port, int *error);
void http_server_free(http_server server);
int http_server_add_route(http_server *server, const char *path,
                          http_method method,
                          void (*handler)(Arena *, http_req, http_res *));

int http_server_start(http_server server);

#endif // SERVER_H
