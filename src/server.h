#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http.h"

#define BUFFER_LEN 1000

typedef struct route {
  const char *path;
  http_method method;
  http_response (*function)(void);
  struct route *next;
} route;

typedef struct {
  int port;
  int socket;
  route *routes;
} server;

int server_init(server *server, int port);
int server_start(server server);
int server_add_route(server *server, const char *path, http_method method,
                     http_response (*function)(void));

#endif  // SERVER_H
