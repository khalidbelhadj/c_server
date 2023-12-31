#include <errno.h>
#include <stdlib.h>

static int counter = 0;

#include "../../src/http_server.h"

void home(http_response *response) {
  FILE *fp = fopen("index.html", "r");
  fread(response->body, 1, HTTP_RES_BODY_SIZE, fp);
  fclose(fp);

  response->status = 200, response->content_type = "text/html";
}

void get_counter(http_response *response) {
  sprintf(response->body,
          "<div>"
          "Clicked %d times"
          "</div>",
          counter);

  response->status = 200;
  response->content_type = "text/html";
}


void increment(http_response *response) {
  counter++;

  sprintf(response->body,
          "<div>"
          "Clicked %d times"
          "</div>",
          counter);

  response->status = 200;
  response->content_type = "text/html";
}

void decrement(http_response *response) {
  counter--;

  sprintf(response->body,
          "<div>"
          "Clicked %d times"
          "</div>",
          counter);

  response->status = 200;
  response->content_type = "text/html";
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

  http_server *server = http_server_new();
  if (http_server_init(server, port) < 0) {
    fprintf(stderr, "[ERROR] Could not initialise server: %s\n",
            strerror(errno));
    exit(1);
  }

  if (http_server_add_route(server, "/", HTTP_GET, &home) < 0) {
    fprintf(stderr, "[ERROR] Could not add route / to server\n");
    exit(1);
  }

  if (http_server_add_route(server, "/get-counter", HTTP_GET, &get_counter) < 0) {
    fprintf(stderr, "[ERROR] Could not add route /get-counter to server\n");
    exit(1);
  }

  if (http_server_add_route(server, "/increment", HTTP_GET, &increment) < 0) {
    fprintf(stderr, "[ERROR] Could not add route /increment to server\n");
    exit(1);
  }

  if (http_server_add_route(server, "/decrement", HTTP_GET, &decrement) < 0) {
    fprintf(stderr, "[ERROR] Could not add route /decrement to server\n");
    exit(1);
  }


  if (http_server_start(*server) < 0) {
    fprintf(stderr, "[ERROR] Server did not terminate properly: %s\n",
            strerror(errno));
    exit(1);
  }

  http_server_free(server);
  return 0;
}
