// Define this to use addrinfo
#define _POSIX_C_SOURCE 200112L

// Define this to use pthread_timedjoin_np
#define _GNU_SOURCE

#include "conn.h"

#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "defer.h"

struct connect_thread_data
{
  char* addr;
  char* port_repr;
  int sockfd;
  struct addrinfo* res;
  bool ok;
};

void* connect_thread_fun(void* data)
{
  struct connect_thread_data* d = (struct connect_thread_data*)data;
  fprintf(stdout, "contacting %s:%s\n", d->addr, d->port_repr);
  int err = connect(d->sockfd, d->res->ai_addr, d->res->ai_addrlen);
  if (err != 0) {
    fprintf(stdout, "connect failed: %s\n", strerror(errno));
    d->ok = false;
    return NULL;
  }
  d->ok = true;
  return NULL;
}

conn* init_conn(char* addr, uint16_t port, int timeout_sec)
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
  DEFER({ freeaddrinfo(res); });

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    fprintf(stderr, "clock_gettime failed: %s\n", strerror(errno));
    return NULL;
  }
  ts.tv_sec += timeout_sec;

  struct connect_thread_data thread_data = {addr, port_repr, sockfd, res};
  pthread_t thread_id;

  pthread_create(&thread_id, NULL, connect_thread_fun, &thread_data);

  int err = pthread_timedjoin_np(thread_id, NULL, &ts);
  if (err != 0) {
    fprintf(stdout, "timeout expired, aborting\n");
    pthread_cancel(thread_id);
    pthread_join(thread_id, NULL);
    return NULL;
  }
  if (!thread_data.ok) {
    fprintf(stderr, "can't connect with %s:%s\n", addr, port_repr);
    return NULL;
  }

  fprintf(stdout, "established a connection with %s:%s\n", addr, port_repr);
  conn* c = malloc(sizeof(conn));
  c->addr = malloc(strlen(addr) + 1);
  strcpy(c->addr, addr);
  c->port = port;
  c->_sockfd = sockfd;
  return c;
}

void free_conn(conn* c)
{
  if (c == NULL) return;
  free(c->addr);
  free(c);
}

struct thread_data
{
  conn* c;
  char* buf;
  size_t buf_size;
  bool ok;
  int recv_bytes;
};

void* send_thread_fun(void* data)
{
  struct thread_data* d = (struct thread_data*)data;
  fprintf(stdout, "sending %lu bytes to %s:%hu\n", d->buf_size, d->c->addr, d->c->port);
  int bytes_sent = send(d->c->_sockfd, d->buf, d->buf_size, 0);
  if (bytes_sent < 0) {
    fprintf(stderr, "send failed: %s\n", strerror(errno));
    d->ok = false;
    return NULL;
  }
  if (bytes_sent == 0) {
    fprintf(stderr, "connection lost\n");
    d->ok = false;
    return NULL;
  }
  if (bytes_sent != (int)d->buf_size) {
    fprintf(stderr, "didn't sent all data: sent %d, want %ld\n", bytes_sent, d->buf_size);
    d->ok = false;
    return NULL;
  }
  fprintf(stdout, "sent %d bytes to %s:%hu\n", bytes_sent, d->c->addr, d->c->port);
  d->ok = true;
  return NULL;
}

bool send_data(conn* c, char* buf, size_t buf_size, int timeout_sec)
{
  if (c == NULL) {
    fprintf(stderr, "connection not initialized\n");
    return false;
  }
  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    fprintf(stderr, "clock_gettime failed: %s\n", strerror(errno));
    return false;
  }
  ts.tv_sec += timeout_sec;

  struct thread_data thread_data = {c, buf, buf_size};
  pthread_t thread_id;

  pthread_create(&thread_id, NULL, send_thread_fun, &thread_data);

  int err = pthread_timedjoin_np(thread_id, NULL, &ts);
  if (err != 0) {
    fprintf(stdout, "timeout expired, aborting\n");
    pthread_cancel(thread_id);
    pthread_join(thread_id, NULL);
    return false;
  }
  return thread_data.ok;
}

void* recv_thread_fun(void* data)
{
  struct thread_data* d = (struct thread_data*)data;
  fprintf(stdout, "receiving at most %lu bytes from %s:%hu\n", d->buf_size, d->c->addr,
          d->c->port);
  int bytes_received = recv(d->c->_sockfd, d->buf, d->buf_size, 0);
  if (bytes_received < 0) {
    fprintf(stderr, "recv failed: %s\n", strerror(errno));
    d->ok = false;
    return NULL;
  }
  if (bytes_received == 0) {
    fprintf(stderr, "connection lost\n");
    d->ok = false;
    return NULL;
  }
  fprintf(stdout, "received %d bytes from %s:%hu\n", bytes_received, d->c->addr, d->c->port);
  d->ok = true;
  d->recv_bytes = bytes_received;
  return NULL;
}

int receive_data(conn* c, char* buf, size_t buf_size, int timeout_sec)
{
  if (c == NULL) {
    fprintf(stderr, "connection not initialized\n");
    return -1;
  }
  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    fprintf(stderr, "clock_gettime failed: %s\n", strerror(errno));
    return -1;
  }
  ts.tv_sec += timeout_sec;

  struct thread_data thread_data = {c, buf, buf_size};
  pthread_t thread_id;

  pthread_create(&thread_id, NULL, recv_thread_fun, &thread_data);

  int err = pthread_timedjoin_np(thread_id, NULL, &ts);
  if (err != 0) {
    fprintf(stdout, "timeout expired, aborting\n");
    pthread_cancel(thread_id);
    pthread_join(thread_id, NULL);
    return -1;
  }
  return thread_data.recv_bytes;
}