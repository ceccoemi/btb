#include "peer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "handshake.h"
#include "message.h"

peer *init_peer(const unsigned char peer_repr[PEER_BLOB_SIZE])
{
  peer *p = malloc(sizeof(peer));
  memcpy(p->address, peer_repr, PEER_ADDR_SIZE);
  p->port = (peer_repr[PEER_ADDR_SIZE] << 8) + peer_repr[PEER_ADDR_SIZE + 1];  // Big endian
  p->sockfd = 0;
  p->bf = NULL;
  return p;
}

void free_peer(peer *p)
{
  if (p == NULL) return;
  free_bitfield(p->bf);
  free(p);
}

int handshake_peer(peer *p, const char peer_id[PEER_ID_LENGTH],
                   const unsigned char info_hash[SHA_DIGEST_LENGTH])
{
  int out_code = 0;

  handshake_msg *h = init_handshake_msg(peer_id, info_hash);
  int sockfd = perform_handshake(p, h);
  if (sockfd <= 0) {
    fprintf(stderr, "perform_handshake failed\n");
    out_code = -1;
    goto exit;
  }
  p->sockfd = sockfd;

exit:
  free_handshake_msg(h);
  return out_code;
}

int receive_bitfield(peer *p)
{
  int out_code = 0;

  if (p->sockfd == 0) {
    fprintf(stderr, "the handshake has not been performed with this peer\n");
    goto exit;
  }

  fprintf(stdout, "reading bitfield\n");
  message *msg = read_message(p->sockfd);
  p->bf = init_bitfield(msg->payload, msg->payload_len);
  fprintf(stdout, "read bitfield done\n");

exit:
  free_message(msg);
  return out_code;
}
