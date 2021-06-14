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

bool handshake_peer(peer *p, const char peer_id[PEER_ID_LENGTH],
                    const unsigned char info_hash[SHA_DIGEST_LENGTH])
{
  bool ok = true;

  handshake_msg *h = init_handshake_msg(peer_id, info_hash);
  int sockfd = perform_handshake(p, h);
  if (sockfd <= 0) {
    fprintf(stderr, "perform_handshake failed\n");
    ok = false;
    goto exit;
  }
  p->sockfd = sockfd;

exit:
  free_handshake_msg(h);
  return ok;
}

bool receive_bitfield(peer *p)
{
  bool ok = true;

  if (p->sockfd == 0) {
    fprintf(stderr, "the handshake has not been performed with this peer\n");
    ok = false;
    goto exit;
  }

  fprintf(stdout, "reading bitfield\n");
  message *msg = read_message(p->sockfd);
  if (msg == NULL) {
    fprintf(stderr, "read_message failed\n");
    ok = false;
    goto exit;
  }
  p->bf = init_bitfield(msg->payload, msg->payload_len);
  fprintf(stdout, "obtained bitfield from peer\n");

exit:
  free_message(msg);
  return ok;
}

bool send_interested(peer *p)
{
  bool ok = true;

  if (p->sockfd == 0) {
    fprintf(stderr, "the handshake has not been performed with this peer\n");
    ok = false;
    goto exit;
  }

  fprintf(stdout, "sending \"Interested\" message\n");
  message *msg = create_message(MSG_INTERESTED, 0, NULL);
  int bytes_sent = send_message(p->sockfd, msg);
  if (bytes_sent <= 0) {
    fprintf(stderr, "send_message failed\n");
    ok = false;
    goto exit;
  }
  fprintf(stdout, "message sent\n");

exit:
  free_message(msg);
  return ok;
}

bool download_piece(peer *p, piece_progress *pp, char *piece_hash)
{
  bool ok = false;

  if (p->sockfd == 0) {
    fprintf(stderr, "the handshake has not been performed with this peer\n");
    goto exit;
  }

  if (p->bf == NULL) {
    fprintf(stderr, "the peer didn't sent the bitfield\n");
    goto exit;
  }

  fprintf(stdout, "trying to download piece #%lu\n", pp->index);

  if (!has_piece(p->bf, pp->index)) {
    fprintf(stderr, "the peer didn't have piece #%lu\n", pp->index);
    goto exit;
  }
  fprintf(stdout, "the peer have piece #%lu\n", pp->index);

  // Send request message
  message *msg = create_message(MSG_REQUEST, , );
  free_message(msg);

  // TODO: download piece

  ok = true;

exit:
  return ok;
}
