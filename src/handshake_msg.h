#pragma once

#include "hash.h"
#include "peer.h"
#include "peer_id.h"
#include "tracker_response.h"

#define HANDSHAKE_RESERVED_LEN 8

typedef struct handshake_msg
{
  size_t pstrlen;
  char *pstr;
  char reserved[HANDSHAKE_RESERVED_LEN];
  unsigned char info_hash[BT_HASH_LENGTH];
  char peer_id[PEER_ID_LENGTH];
} handshake_msg;

handshake_msg *init_handshake_msg(unsigned char info_hash[BT_HASH_LENGTH],
                                  char peer_id[PEER_ID_LENGTH]);

void free_handshake_msg(handshake_msg *);

// Encode the handshake message in the buffer buf.
// It returns the number of bytes written in the buffer.
// It returns < 0 if it fails.
int encode_handshake_msg(handshake_msg *, char *buf);

// Decode a handshake_msg from buf.
// It returns NULL if it fails.
handshake_msg *decode_handshake_msg(char *buf, size_t buf_length);