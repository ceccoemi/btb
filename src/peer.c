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
  bool ok = true;

  if (p->sockfd == 0) {
    fprintf(stderr, "the handshake has not been performed with this peer\n");
    ok = false;
    goto exit;
  }

  if (p->bf == NULL) {
    fprintf(stderr, "the peer didn't sent the bitfield\n");
    ok = false;
    goto exit;
  }

  fprintf(stdout, "trying to download piece #%lu\n", pp->index);

  if (!has_piece(p->bf, pp->index)) {
    fprintf(stderr, "the peer didn't have piece #%lu\n", pp->index);
    ok = false;
    goto exit;
  }
  fprintf(stdout, "the peer have piece #%lu\n", pp->index);

  size_t block_size = 16384;  // 2^14 bytes
  while (pp->downloaded <= pp->size) {
    // Send request message
    size_t payload_len = 12;  // 4 bytes piece index + 4 bytes block offset + 4 bytes block length
    unsigned char *payload = malloc(payload_len);
    // Convert to a Big Endian representation.
    payload[0] = pp->index >> 24;
    payload[1] = pp->index >> 16;
    payload[2] = pp->index >> 8;
    payload[3] = pp->index;
    size_t block_offset = pp->downloaded;
    payload[4] = block_offset >> 24;
    payload[5] = block_offset >> 16;
    payload[6] = block_offset >> 8;
    payload[7] = block_offset;
    payload[8] = block_size >> 24;
    payload[9] = block_size >> 16;
    payload[10] = block_size >> 8;
    payload[11] = block_size;
    fprintf(stdout, "requesting a block with offset %lu bytes of piece %lu\n", block_offset,
            pp->index);
    message *msg_request = create_message(MSG_REQUEST, payload_len, payload);
    free(payload);
    bool ok = send_message(p->sockfd, msg_request);
    free_message(msg_request);
    if (!ok) {
      fprintf(stderr, "send_message failed\n");
      ok = false;
      goto exit;
    }
    message *msg = read_message(p->sockfd);
    if (msg == NULL) {
      fprintf(stderr, "read_message failed\n");
      ok = false;
      goto exit;
    }
    if (msg->id != MSG_PIECE) {
      fprintf(stdout,
              "the peer didn't reply with a \"Piece\" message, got message ID %u. Skipping...\n",
              msg->id);
      free_message(msg);
      continue;
    }
    pp->downloaded += block_size;
    free_message(msg);
  }

exit:
  return ok;
}
