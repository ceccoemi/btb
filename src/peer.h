#pragma once

#include <openssl/sha.h>
#include <stdbool.h>
#include <stdint.h>

#include "bitfield.h"
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
  // This is set in the receive_bitfield function.
  bitfield *bf;
} peer;

peer *init_peer(const unsigned char[PEER_BLOB_SIZE]);

void free_peer(peer *p);

// Perform the handshake with the input peer.
// This method must be called before sending and/or receveing messages
// It returns 0 if it succeeds.
int handshake_peer(peer *, const char[PEER_ID_LENGTH], const unsigned char[SHA_DIGEST_LENGTH]);

// Receive the bitfield of the peer.
// This function must be called after perfoming the handshake.
int receive_bitfield(peer *);

// Send an "Interested" message to the input peer.
// This message should be sent before start to download pieces.
int send_interested(peer *);

// Try to download the piece with the specified index.
bool download_piece(peer *, size_t piece_index, char *piece_hash);
