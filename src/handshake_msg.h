#pragma once

#include "hash.h"
#include "peer.h"
#include "peer_id.h"
#include "tracker_response.h"

typedef struct handshake_msg
{
  size_t pstrlen;
  char *pstr;
} handshake_msg;

handshake_msg *init_handshake_msg(unsigned char info_hash[BT_HASH_LENGTH],
                                  char peer_id[PEER_ID_LENGTH]);

void free_handshake_msg(handshake_msg *);