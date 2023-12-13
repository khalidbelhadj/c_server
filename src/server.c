#include "server.h"
#include <string.h>

server *current_server = {0};

int server_init(server *server, int port) {
  // Creating the socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    return -1;
  }

  // Creating socket address
  struct sockaddr_in address = {0};
  bzero(&address, sizeof(address));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);

  // Binding the socket
  int bind_result = bind(sock, (struct sockaddr *)&address, sizeof(address));
  if (bind_result < 0) {
    return -1;
  }

  int listen_result = listen(sock, 10);
  if (listen_result < 0) {
    return -1;
  }
  printf("[INFO] Server listening on port %d\n", port);

  server->port = port;
  server->routes = NULL;
  server->socket = sock;

  return 0;
}

http_response _server_get_response(server server, http_request request_buffer) {
  http_response response = {0};
  route *current = server.routes;

  while (current != NULL) {
    if (strcmp(current->path, request_buffer.path) == 0 && current->method == request_buffer.method) {
      response = current->function();
      break;
    }
    current = current->next;
  }

  if (response.status == 0) {
    response.status = 404;
    response.content_type = "text/plain";
    response.body = "404 Not Found";
  }

  return response;
}
void _server_sigint_handler(int signo) {
  switch (signo) {
  case SIGINT:
    printf("[INFO] SIGINT recieved\n");
    printf("[INFO] Closing server\n");
    close(current_server->socket);
    exit(0);
  }
}

int server_start(server server) {
  current_server = &server;

  if (signal(SIGINT, _server_sigint_handler) == SIG_ERR) {
    perror("Error setting up signal handler");
    exit(EXIT_FAILURE);
  }

  printf("[INFO] Server started\n");

  // Reading messages
  char request_buffer[BUFFER_LEN + 1];
  char response_buffer[BUFFER_LEN + 1];

  memset(request_buffer, 0, BUFFER_LEN + 1);
  memset(response_buffer, 0, BUFFER_LEN + 1);

  int other_sock;
  int n;

  while (1) {
    other_sock = accept(server.socket, (struct sockaddr *)NULL, NULL);

    while ((n = read(other_sock, request_buffer, BUFFER_LEN - 2)) > 0) {
      http_request request = http_request_from_string(request_buffer);

      char request_str[BUFFER_LEN + 1];
      http_request_to_string(request_str, request);
      printf("[INFO] Recieved request:\n%s\n", request_str);

      http_response response = _server_get_response(server, request);
      http_response_to_string(response_buffer, response);

      int write_result =
          send(other_sock, response_buffer, strlen(response_buffer), 0);

      if (write_result < 0) {
        return -1;
      }

      close(other_sock);

      memset(request_buffer, 0, BUFFER_LEN + 1);
      memset(response_buffer, 0, BUFFER_LEN + 1);
    }
  }
  printf("[INFO] Closing server");
  close(server.socket);
  return 0;
}

int server_add_route(server *server, const char *path, http_method method, http_response (*function)(void)) {
  route *new_route = calloc(1, sizeof(route));

  if (new_route == NULL) {
    return -1;
  }

  new_route->path = path;
  new_route->function = function;
  new_route->next = NULL;
  new_route->method = method;

  if (server->routes == NULL) {
    server->routes = new_route;
  } else {
    route *current = server->routes;

    while (current->next != NULL) {
      current = current->next;
    }

    current->next = new_route;
  }

  printf("[INFO] Added route %s to server\n", path);
  return 0;
}
