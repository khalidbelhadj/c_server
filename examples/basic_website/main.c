#include <errno.h>

#include "../../src/http_server.h"

void hello(Arena *a, http_req req, http_res *res) {
    (void)req;
    http_res_file(a, res, "hello.html");
}

void bye(Arena *a, http_req req, http_res *res) {
    (void) req;
    http_res_file(a, res, "bye.html");
}

void home(Arena *a, http_req req, http_res *res) {
    (void) req;
    http_res_file(a, res, "index.html");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "[ERROR] Port not provided\n");
        printf("usage:\n");
        printf("    ./main <port>\n");
        return 1;
    }

    char *end;
    size_t port = strtod(argv[1], &end);

    int error = 0;
    http_server server = http_server_new(port, &error);
    if (error != 0) {
        fprintf(stderr, "[ERROR] Could not create a server on port %zu\n",
                port);
        exit(1);
    }

    {

        if (http_server_add_route(&server, "/hello", HTTP_GET, &hello) < 0) {
            fprintf(stderr, "[ERROR] Could not add route /hello to server\n");
            exit(1);
        }

        if (http_server_add_route(&server, "/bye", HTTP_GET, &bye) < 0) {
            fprintf(stderr, "[ERROR] Could not add route /bye to server\n");
            exit(1);
        }

        if (http_server_add_route(&server, "/", HTTP_GET, &home) < 0) {
            fprintf(stderr, "[ERROR] Could not add route / to server\n");
            exit(1);
        }
    }

    if (http_server_start(server) < 0) {
        fprintf(stderr, "[ERROR] Server did not terminate properly: %s\n",
                strerror(errno));
        exit(1);
    }

    http_server_free(server);
    return 0;
}
