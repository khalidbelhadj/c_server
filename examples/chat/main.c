#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/http_server.h"

typedef struct {
    char content[100];
} Chat;

Chat chats[1024];
static int chats_count = 0;
static int chats_content_len = 0;

FILE *out_file = NULL;

void new_chat(Arena *a, http_req req, http_res *res) {
    if (out_file == NULL) {
        fprintf(stderr, "could not append to storage\n");
    } else {
        printf("Appended %s\n", req.body);
        fputs(req.body, out_file);
        fputs("\n", out_file);
        fflush(out_file);
    }

    chats[chats_count] = (Chat){};
    strcpy(chats[chats_count].content, req.body);
    ++chats_count;
    chats_content_len += strlen(req.body);
    res->body = "";
    res->content_type = "text/html";
    res->status = HTTP_STATUS_OK;
}

void get_chats(Arena *a, http_req req, http_res *res) {
    char *body = arena_alloc(a, sizeof(char) * (chats_content_len + 5));

    for (int i = chats_count - 1; i >= 0; --i) {
        char xs[100];
        snprintf(xs, 100, "%s\n", chats[i].content);
        strcat(body, xs);
    }

    res->body = body;
    res->content_type = "text/html";
    res->status = HTTP_STATUS_OK;
}

void home(Arena *a, http_req req, http_res *res) {
    http_res_file(a, res, "index.html");
}

void username(Arena *a, http_req req, http_res *res) {
    http_res_file(a, res, "username.html");
}

void read_chats() {
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    out_file = fopen("chats.txt", "r");

    if (out_file == NULL) {
        printf("couldn't open file");
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, out_file)) != -1) {
        chats[chats_count] = (Chat){};
        strcpy(chats[chats_count].content, line);
        chats_count++;
    }

    fclose(out_file);
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

    read_chats();

    out_file = fopen("chats.txt", "a");

    http_server server = http_server_new(port, NULL);

    http_server_add_route(&server, "/", HTTP_GET, &home);
    http_server_add_route(&server, "/username", HTTP_GET, &username);
    http_server_add_route(&server, "/new-chat", HTTP_POST, &new_chat);
    http_server_add_route(&server, "/get-chats", HTTP_GET, &get_chats);

    http_server_start(server);
    http_server_free(server);

    return 0;
}
