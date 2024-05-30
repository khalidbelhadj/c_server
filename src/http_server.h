#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http.h"

#define BUFFER_LEN 2024

typedef struct route {
    const char *path;
    http_method method;
    void (*handler)(Arena *, http_req, http_res *);
    struct route *next;
} route;

typedef struct {
    int port;
    int server_fd;
    route *routes;
    char *base;
} http_server;

void http_server_free(http_server server);
int http_server_init(http_server *server, int port);
int http_server_start(http_server server);
int http_server_add_route(http_server *server, const char *path,
                          http_method method,
                          void (*handler)(Arena *, http_req, http_res *));

#endif // SERVER_H
