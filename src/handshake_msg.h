#pragma once

#include <openssl/sha.h>

#include "peer.h"
#include "tracker_response.h"

typedef struct handshake_msg
{
} handshake_msg;

handshake_msg *init_handshake_msg(const unsigned char info_hash[SHA_DIGEST_LENGTH]);

void free_handshake_msg(handshake_msg *);