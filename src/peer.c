#include "peer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "big_endian.h"

peer *init_peer(unsigned char peer_repr[PEER_BLOB_SIZE])
{
  peer *p = malloc(sizeof(peer));
  memcpy(p->address, peer_repr, PEER_ADDR_SIZE);
  p->port = big_endian_to_lu((unsigned char *)peer_repr + PEER_ADDR_SIZE, PEER_PORT_SIZE);
  return p;
}

void free_peer(peer *p)
{
  if (p == NULL) return;
  free(p);
}