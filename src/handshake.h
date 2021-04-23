#pragma once

#include <openssl/sha.h>

#include "peer.h"
#include "tracker_response.h"

int perform_handshake(peer *, const unsigned char[PEER_ID_LENGTH],
                      const unsigned char[SHA_DIGEST_LENGTH]);
