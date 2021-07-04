#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct conn
{
  // Socket file descriptor
  int _sockfd;
} conn;

// Establish a connection with the specified address at the input port.
// It returns the conn struct with which it will be possible to send and receive data.
conn* init_conn(char* addr, uint16_t port);

void free_conn(conn* c);

// Send buf_size bytes of data from buf into the specified connection.
// It stops after timeout_sec seconds.
// It returns true if it succeeds.
bool send_data(conn* c, char* buf, size_t buf_size, int timeout_sec);

// Receive buf_size bytes of data in buf from the specified connection.
// It stops after timeout_sec seconds.
// It returns true if it succeeds.
bool receive_data(conn* c, char* buf, size_t buf_size, int timeout_sec);
