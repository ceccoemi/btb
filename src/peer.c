#include "peer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

peer *init_peer(const unsigned char peer_repr[PEER_BLOB_SIZE])
{
  peer *p = malloc(sizeof(peer));
  for (int i = 0; i < PEER_ADDR_SIZE; i++) {
    p->address[i] = peer_repr[i];
  }
  p->port = (peer_repr[PEER_ADDR_SIZE] << 8) + peer_repr[PEER_ADDR_SIZE + 1];  // Big endian
  return p;
}

void free_peer(peer *p)
{
  if (p != NULL) {
    free(p);
  }
}
