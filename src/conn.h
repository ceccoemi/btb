#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct conn
{
  // Null-terminated string of an IP address or a domain name.
  char* addr;
  uint16_t port;
  // Socket file descriptor
  int _sockfd;
} conn;

// Establish a connection with the specified address at the input port.
// addr must be a null-terminated string of the format "xxx.xxx.xxx.xxx" or a domain name.
// It returns the conn struct with which it will be possible to send and receive data.
// It aborts after timeout_sec seconds.
// It returns NULL if it fails.
conn* init_conn(char* addr, uint16_t port, int timeout_sec);

void free_conn(conn* c);

// Send buf_size bytes of data from buf into the specified connection.
// It aborts after timeout_sec seconds.
// It returns true if it succeeds.
bool send_data(conn* c, char* buf, size_t buf_size, int timeout_sec);

// Receive buf_size bytes of data in buf from the specified connection.
// It aborts after timeout_sec seconds.
// It returns the number of received bytes: bytes received <= 0 means that it failed.
int receive_data(conn* c, char* buf, size_t buf_size, int timeout_sec);
