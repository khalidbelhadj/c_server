#include "http.h"
#include <string.h>

void http_response_to_string(char *dst, http_response response) {
  snprintf(dst, 1000, HTTP_VERSION" %d OK\nContent-Type: %s\n\r\n\r\n%s", response.status, response.content_type, response.body);
}

http_request http_request_from_string(char *request) {
  http_request req = {0};
  char *line = strtok(request, "\n");

  char *method = strtok(line, " ");
  if (strcmp(method, "GET") == 0) {
    req.method = HTTP_GET;
  } else if (strcmp(method, "POST") == 0) {
    req.method = HTTP_POST;
  } else {
    printf("Unknown method: %s\n", method);
    exit(1);
  }

  char *path = strtok(NULL, " ");
  req.path = path;

  char *version = strtok(NULL, " ");
  req.version = version;

  return req;
}

void http_request_to_string(char *dst, http_request request) {
  snprintf(dst, 1000, "method: %s\npath: %s\nversion: %s\n", request.method == HTTP_GET ? "GET" : "POST", request.path, request.version);
}
