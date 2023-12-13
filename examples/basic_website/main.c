#include <errno.h>

#include "../../src/server.h"

http_response hello(void) {
  FILE *fp = fopen("hello.html", "r");
  char *file_content = calloc(1, 100000);
  fread(file_content, 1, 100000, fp);
  fclose(fp);

  http_response response = {
      .status = 200,
      .content_type = "text/html",
      .body = file_content,
  };
  return response;
}

http_response bye(void) {
  FILE *fp = fopen("bye.html", "r");
  char *file_content = calloc(1, 100000);
  fread(file_content, 1, 100000, fp);
  fclose(fp);

  http_response response = {
      .status = 200,
      .content_type = "text/html",
      .body = file_content,
  };
  return response;
}

http_response home(void) {
  FILE *fp = fopen("index.html", "r");
  char *file_content = calloc(1, 100000);
  fread(file_content, 1, 100000, fp);
  fclose(fp);

  http_response response = {
      .status = 200,
      .content_type = "text/html",
      .body = file_content,
  };
  return response;
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

  server *server = calloc(1, sizeof(server));
  if (server_init(server, port) < 0) {
    fprintf(stderr, "[ERROR] Could not initialise server: %s\n",
            strerror(errno));
    exit(1);
  }

  if (server_add_route(server, "/hello", HTTP_GET, &hello) < 0) {
    fprintf(stderr, "[ERROR] Could not add route /hello to server\n");
    exit(1);
  }

  if (server_add_route(server, "/bye", HTTP_GET, &bye) < 0) {
    fprintf(stderr, "[ERROR] Could not add route /bye to server\n");
    exit(1);
  }

  if (server_add_route(server, "/", HTTP_GET, &home) < 0) {
    fprintf(stderr, "[ERROR] Could not add route /hello to server\n");
    exit(1);
  }

  if (server_start(*server) < 0) {
    fprintf(stderr, "[ERROR] Server did not terminate properly: %s\n",
            strerror(errno));
    exit(1);
  }

  return 0;
}
