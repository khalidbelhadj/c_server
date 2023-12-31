#include "http.h"

#include <string.h>

http_response *http_response_new(void) {
  http_response *response = calloc(1, sizeof(http_response));
  response->body = calloc(1, sizeof(char) * HTTP_RES_BODY_SIZE);
  return response;
}

void http_response_free(http_response *response) {
  free(response->body);
  free(response);
}

void http_response_stringify(char *dst, http_response response) {
  snprintf(dst, 1000, HTTP_VERSION " %d OK\nContent-Type: %s\n\r\n\r\n %s",
           response.status, response.content_type, response.body);
}

http_request http_request_parse(char *request) {
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

  char *version = strtok(NULL, "\r\n\r\n");
  req.version = version;

  return req;
}

void http_request_stringify(char *dst, http_request request) {
  sprintf(dst, "{ method: %s, path: %s, version: %s }",
          request.method == HTTP_GET ? "GET" : "POST", request.path,
          request.version);
}
