#include "peer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

peer *init_peer(const unsigned char peer_repr[PEER_BLOB_SIZE])
{
  peer *p = malloc(sizeof(peer));
  memcpy(p->address, peer_repr, PEER_ADDR_SIZE);
  p->port = (peer_repr[PEER_ADDR_SIZE] << 8) + peer_repr[PEER_ADDR_SIZE + 1];  // Big endian
  p->sockfd = 0;
  return p;
}

void free_peer(peer *p) { free(p); }

int handshake_peer(peer *p, const char peer_id[PEER_ID_LENGTH],
                   const unsigned char info_hash[SHA_DIGEST_LENGTH])
{
  return -1;
}
