#pragma once

#include <stdint.h>

// Number of bytes to represent a peer address
#define PEER_ADDR_SIZE 4
// Number of bytes to represent a peer port
#define PEER_PORT_SIZE 2
// Number of bytes to represent a peer (address + port)
#define PEER_BLOB_SIZE (PEER_ADDR_SIZE + PEER_PORT_SIZE)

typedef struct
{
  uint16_t port;
  char address[PEER_ADDR_SIZE];
} peer;

peer *init_peer(char peer_repr[PEER_BLOB_SIZE]);

void free_peer(peer *p);