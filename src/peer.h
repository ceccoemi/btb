#pragma once

#include <stdint.h>

#define PEER_BLOB_SIZE 6  // number of bytes to represent a peer
#define PEER_ADDR_SIZE 4

typedef struct
{
  uint16_t port;
  unsigned char address[PEER_ADDR_SIZE];
} peer;

peer *init_peer(const unsigned char[PEER_BLOB_SIZE]);

void free_peer(peer *p);
