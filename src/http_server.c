#include "http_server.h"

#include "http.h"
#include <pthread.h>

http_server *current_server = NULL;

void http_server_free(http_server server) {
    route *current = server.routes;
    route *next = NULL;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
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

    *server = (http_server){.port = port, .routes = NULL, .server_fd = sock};

    return 0;
}

http_res *_http_server_get_res(Arena *a, http_server server, http_req req) {
    http_res *res = arena_alloc(a, sizeof(http_res));
    route *current = server.routes;
    bool route_found = false;

    while (current != NULL) {
        if (strcmp(current->path, req.path) == 0 &&
            current->method == req.method) {
            current->handler(a, req, res);
            route_found = true;
            break;
        }
        current = current->next;
    }

    if (!route_found) {
        http_res_not_found(res);
    }
    return res;
}

void _http_server_sigint_handler(int signo) {
    (void)signo;
    printf("[INFO] Closing server\n");
    close(current_server->server_fd);
    exit(0);
}

typedef struct {
    int client_fd;
    http_server server;
} thread_args;

void *_handle_client(void *args) {
    char req_buffer[BUFFER_LEN + 1] = {0};
    char res_buffer[BUFFER_LEN + 1] = {0};

    thread_args t = *((thread_args *)args);
    int bytes_recieved = recv(t.client_fd, req_buffer, BUFFER_LEN - 2, 0);

    if (bytes_recieved <= 0) {
        close(t.client_fd);
        close(t.server.server_fd);
        return NULL;
    }

    Arena a = {0};

    http_req req = http_req_parse(&a, req_buffer);
    char *req_str = http_req_stringify(&a, req);

    printf("[INFO] Recieved req: %s\n", req_str);

    http_res *res = _http_server_get_res(&a, t.server, req);
    char *res_str = http_res_stringify(&a, *res);

    int write_result = send(t.client_fd, res_str, strlen(res_str), 0);

    if (write_result < 0) {
        close(t.client_fd);
        close(t.server.server_fd);
        return NULL;
    }

    arena_free(&a);
    close(t.client_fd);
    return NULL;
}

int http_server_start(http_server server) {
    current_server = &server;

    if (signal(SIGINT, _http_server_sigint_handler) == SIG_ERR) {
        perror("Error setting up signal handler");
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Server started on http://localhost:%d\n", server.port);

    while (true) {
        int client_fd = accept(server.server_fd, (struct sockaddr *)NULL, NULL);

        pthread_t thread;
        thread_args t = (thread_args){client_fd, server};
        pthread_create(&thread, NULL, _handle_client, (void *)&t);
        pthread_detach(thread);
    }

    printf("[INFO] Closing server");
    close(server.server_fd);
    return 0;
}

int http_server_add_route(http_server *server, const char *path,
                          http_method method,
                          void (*handler)(Arena *a, http_req, http_res *)) {
    route *new_route = calloc(1, sizeof(route));

    if (new_route == NULL) {
        return -1;
    }

    new_route->path = path;
    new_route->handler = handler;
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
