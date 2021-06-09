#pragma once

#include <openssl/sha.h>
#include <stdint.h>

#include "peer_id.h"

#define PEER_BLOB_SIZE 6  // number of bytes to represent a peer
#define PEER_ADDR_SIZE 4

typedef struct
{
  uint16_t port;
  unsigned char address[PEER_ADDR_SIZE];
  // Socket to connect with the peer.
  // It is set in the handshake_peer function.
  int sockfd;
} peer;

peer *init_peer(const unsigned char[PEER_BLOB_SIZE]);

void free_peer(peer *p);

// Perform the handshake with the input peer.
// This method must be called before sending and/or receveing messages
// It returns 0 if it succeeds.
int handshake_peer(peer *, const char[PEER_ID_LENGTH], const unsigned char[SHA_DIGEST_LENGTH]);
