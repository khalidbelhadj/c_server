#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int counter = 0;

#include "../../src/http_server.h"

typedef struct {
    char content[100];
    char *time;
} Chat;

Chat chats[1024];
static int chats_count = 0;

void new_chat(Arena *a, http_req req, http_res *res) {
    chats[chats_count] = (Chat){.time = "12:30"};
    strcpy(chats[chats_count].content, req.query_params->value);
    ++chats_count;

    res->body = arena_alloc(a, sizeof(char) * 10000);
    for (int i = chats_count - 1; i >= 0; --i) {
        char xs[100];
        snprintf(xs, 100, "<div class='chat'>%s</div><br>\n", chats[i].content);
        strcat(res->body, xs);
    }
    res->content_type = "text/html";
    res->status = HTTP_STATUS_OK;
}

void get_chats(Arena *a, http_req req, http_res *res) {
    res->body = arena_alloc(a, sizeof(char) * 10000);
    for (int i = chats_count - 1; i >= 0; --i) {
        char xs[100];
        snprintf(xs, 100, "<div class='chat'>%s</div><br>\n", chats[i].content);
        strcat(res->body, xs);
    }
    res->content_type = "text/html";
    res->status = HTTP_STATUS_OK;
}

void home(Arena *a, http_req req, http_res *res) {
    http_res_file(a, res, "index.html");
}

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
    if (http_server_init(&server, port) < 0) {
        fprintf(stderr, "[ERROR] Could not initialise server: %s\n",
                strerror(errno));
        exit(1);
    }

    if (http_server_add_route(&server, "/", HTTP_GET, &home) < 0) {
        fprintf(stderr, "[ERROR] Could not add route / to server\n");
        exit(1);
    }

    if (http_server_add_route(&server, "/new-chat", HTTP_GET, &new_chat) < 0) {
        fprintf(stderr, "[ERROR] Could not add route /new_chat to server\n");
        exit(1);
    }

    if (http_server_add_route(&server, "/get-chats", HTTP_GET, &get_chats) < 0) {
        fprintf(stderr, "[ERROR] Could not add route /new_chat to server\n");
        exit(1);
    }

    if (http_server_start(server) < 0) {
        fprintf(stderr, "[ERROR] Server did not terminate properly: %s\n",
                strerror(errno));
        exit(1);
    }

    http_server_free(server);
    return 0;
}
