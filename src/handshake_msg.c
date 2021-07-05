#define _POSIX_C_SOURCE 200112L

#include "handshake_msg.h"

#include <errno.h>
#include <netdb.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "peer.h"
#include "peer_id.h"

handshake_msg *init_handshake_msg(const unsigned char info_hash[SHA_DIGEST_LENGTH])
{
  handshake_msg *h = malloc(sizeof(handshake_msg));
  return h;
}

void free_handshake_msg(handshake_msg *h)
{
  if (h == NULL) return;
  free(h);
}

