#pragma once

#include <openssl/sha.h>

#include "peer.h"
#include "tracker_response.h"

#define HANDSHAKE_TIMEOUT_MSEC 5000

typedef struct handshake_msg
{
  char *msg;
  size_t length;
  // Address where the info hash string starts, which is of SHA_DIGEST_LENGTH bytes.
  char *_info_hash_start;
} handshake_msg;

handshake_msg *init_handshake_msg(const unsigned char[SHA_DIGEST_LENGTH]);

void free_handshake_msg(handshake_msg *);

// Perform the handshake with the input peer.
// It returns the socket file descriptor if succeded, otherwise it returns -1.
int perform_handshake(peer *, handshake_msg *);
