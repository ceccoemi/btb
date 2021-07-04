#pragma once

#include <openssl/sha.h>

#include "peer.h"
#include "tracker_response.h"

typedef struct handshake_msg
{
  char *msg;
  size_t length;
  // Address where the info hash string starts, which is of SHA_DIGEST_LENGTH bytes.
  char *_info_hash_start;
} handshake_msg;

handshake_msg *init_handshake_msg(const unsigned char[SHA_DIGEST_LENGTH]);

void free_handshake_msg(handshake_msg *);