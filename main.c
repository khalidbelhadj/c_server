
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_LEN 1000

int server_init(int port) {
  // Creating the socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    fprintf(stderr, "[ERROR] Could not create socket: %s\n", strerror(errno));
    exit(1);
  }
  printf("[INFO] Socket successfully created\n");

  // Creating socket address
  struct sockaddr_in address = {0};
  bzero(&address, sizeof(address));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);

  // Binding the socket
  int bind_result = bind(sock, (struct sockaddr *)&address, sizeof(address));
  if (bind_result < 0) {
    fprintf(stderr, "[ERROR] Could not bind socket to address at port %d: %s\n",
            port, strerror(errno));
    exit(1);
  }
  printf("[INFO] Socket successfully bound\n");

  int listen_result = listen(sock, 10);
  if (listen_result < 0) {
    fprintf(stderr, "[ERROR] Could not listen on socket: %s\n",
            strerror(errno));
    exit(1);
  }
  printf("[INFO] Socket successfully listening\n");
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


  // Accept only one connection
  printf("[INFO] Waiting for connection on port %d\n", port);
  int other_sock = accept(sock, (struct sockaddr *)NULL, NULL);

  // Reading messages
  char input_buffer[BUFFER_LEN + 1];
  char response_buffer[BUFFER_LEN + 1];
  memset(input_buffer, 0, BUFFER_LEN + 1);
  memset(response_buffer, 0, BUFFER_LEN + 1);
  int n;

  while ((n = read(other_sock, input_buffer, BUFFER_LEN - 1)) > 0) {
    input_buffer[n + 1] = '\0';
    printf("Message: %s", input_buffer);

    snprintf(response_buffer, sizeof(response_buffer),
        "HTTP/1.1 200 OK \
        Content-Type: text/html\r\n\r\n\
        <h1>this is from C server</h1>");

    int write_result =
        send(other_sock, response_buffer, strlen(response_buffer), NULL);
    if (write_result < 0) {
      fprintf(stderr, "[ERROR] Could not write a response: %s\n",
              strerror(errno));
      exit(1);
    }

    memset(input_buffer, 0, BUFFER_LEN + 1);
    memset(response_buffer, 0, BUFFER_LEN + 1);
  }

  close(other_sock);
  close(sock);

  return 0;
}
