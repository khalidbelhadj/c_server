#include <errno.h>

#include "../../src/http_server.h"

void hello(Arena *a, http_res *res) { http_res_file(a, res, "hello.html"); }

void bye(Arena *a, http_res *res) { http_res_file(a, res, "bye.html"); }

void home(Arena *a, http_res *res) { http_res_file(a, res, "index.html"); }

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "[ERROR] Port not provided\n");
        printf("usage:\n");
        printf("    ./main <port>\n");
        exit(1);
    }

    char *end;
    int port = strtod(argv[1], &end);

    http_server server = {0};

    {
        if (http_server_init(&server, port) < 0) {
            fprintf(stderr, "[ERROR] Could not initialise server: %s\n",
                    strerror(errno));
            exit(1);
        }

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
