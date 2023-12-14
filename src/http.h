#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdlib.h>

#define HTTP_VERSION "HTTP/1.0"
#define HTTP_RES_BODY_SIZE 1024

typedef enum {
  HTTP_GET,
  HTTP_POST,
} http_method;

typedef struct {
  char *key;
  char *value;
} http_header;

typedef struct {
  int status;
  char *content_type;
  char *body;
  http_header *headers;
} http_response;

typedef struct {
  http_method method;
  char *path;
  char *version;
  http_header *headers;
} http_request;

http_response *http_response_new(void);
void http_response_free(http_response *response);
void http_response_to_string(char *dst, http_response response);
http_request http_request_from_string(char *request);
void http_request_to_string(char *dst, http_request request);

#endif  // HTTP_H
