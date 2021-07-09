// Define this to use addrinfo
#define _POSIX_C_SOURCE 200112L

#include "conn.h"

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

conn* init_conn(char* addr, uint16_t port)
{
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));  // Make sure that the struct is empty
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo* res;
  char port_repr[6];  // Maximum port number value is 5 digits
  sprintf(port_repr, "%hu", port);
  int out = getaddrinfo(addr, port_repr, &hints, &res);
  if (out != 0) {
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(out));
    return NULL;
  }

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  fprintf(stdout, "contacting %s:%s\n", addr, port_repr);
  int err = connect(sockfd, res->ai_addr, res->ai_addrlen);
  freeaddrinfo(res);
  if (err != 0) {
    fprintf(stdout, "connect failed: %s\n", strerror(errno));
    return NULL;
  }
  fprintf(stdout, "established a connection with %s:%s\n", addr, port_repr);
  conn* c = malloc(sizeof(conn));
  c->addr = malloc(strlen(addr));
  strcpy(c->addr, addr);
  c->_sockfd = sockfd;
  return c;
}

void free_conn(conn* c)
{
  if (c == NULL) return;
  free(c->addr);
  free(c);
}

bool send_data(conn* c, char* buf, size_t buf_size, int timeout_sec)
{
  fprintf(stdout, "sending data to %s:%hu\n", c->addr, c->port);
  int bytes_sent = send(c->_sockfd, buf, buf_size, 0);
  if (bytes_sent < 0) {
    fprintf(stderr, "send failed: %s\n", strerror(errno));
    return false;
  }
  if (bytes_sent == 0) {
    fprintf(stderr, "connection lost\n");
    return false;
  }
  if (bytes_sent != (int)buf_size) {
    fprintf(stderr, "didn't sent all data: sent %d, want %ld\n", bytes_sent, buf_size);
    return false;
  }
  return true;
}

bool receive_data(conn* c, char* buf, size_t buf_size, int timeout_sec) { return false; }