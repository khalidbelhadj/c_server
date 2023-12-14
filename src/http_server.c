#include "http_server.h"

#include <stdlib.h>
#include <string.h>

#include "http.h"

http_server *current_server = NULL;

http_server *http_server_new(void) {
  return (http_server *)calloc(1, sizeof(http_server));
  ;
}

void http_server_free(http_server *server) {
  route *current = server->routes;
  route *next = NULL;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  free(server);
}

int http_server_init(http_server *server, int port) {
  current_server = server;

  // Creating the socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    return -1;
  }

  // Set SO_REUSEADDR option to allow reuse of local address
  int reuseaddr = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,
                 sizeof(reuseaddr)) == -1) {
    perror("Error setting SO_REUSEADDR option");
    close(sock);
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

  *server = (http_server){.port = port, .routes = NULL, .socket = sock};

  return 0;
}

void _http_server_get_response(http_server server, http_request request_buffer,
                               http_response *response) {
  route *current = server.routes;

  while (current != NULL) {
    if (strcmp(current->path, request_buffer.path) == 0 &&
        current->method == request_buffer.method) {
      current->function(response);
      break;
    }
    current = current->next;
  }

  if (response->status == 0) {
    response->status = 404;
    response->content_type = "text/plain";
    strcpy(response->body, "404 Not Found");
  }
}

void _http_server_sigint_handler(int signo) {
  (void)signo;
  printf("[INFO] Closing server\n");
  close(current_server->socket);
  exit(0);
}

int http_server_start(http_server server) {
  current_server = &server;

  if (signal(SIGINT, _http_server_sigint_handler) == SIG_ERR) {
    perror("Error setting up signal handler");
    exit(EXIT_FAILURE);
  }

  printf("[INFO] Server started on http://localhost:%d\n", server.port);

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

      char request_str[BUFFER_LEN + 1] = {0};
      http_request_to_string(request_str, request);
      printf("[INFO] Recieved request: %s\n", request_str);

      http_response *response = http_response_new();
      _http_server_get_response(server, request, response);
      http_response_to_string(response_buffer, *response);
      http_response_free(response);

      int write_result =
          send(other_sock, response_buffer, strlen(response_buffer), 0);

      close(other_sock);

      if (write_result < 0) {
        return -1;
      }

      memset(request_buffer, 0, BUFFER_LEN + 1);
      memset(response_buffer, 0, BUFFER_LEN + 1);
    }
  }
  printf("[INFO] Closing server");
  close(server.socket);
  return 0;
}

int http_server_add_route(http_server *server, const char *path,
                          http_method method,
                          void (*function)(http_response *)) {
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
