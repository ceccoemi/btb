#include "client.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "conn.h"
#include "handshake_msg.h"
#include "message.h"
#include "peer.h"
#include "peer_id.h"
#include "pieces_pool.h"
#include "torrent_file.h"
#include "tracker_response.h"

#define TIMEOUT_SEC 5

bool download_torrent(const char *filename)
{
  bool ok = false;
  torrent_file *tf = init_torrent_file();
  ok = parse_torrent_file(tf, filename);
  if (!ok) {
    fprintf(stderr, "parse_torrent_file failed\n");
    goto exit_0;
  }
  fprintf(stdout,
          "the file is %lu bytes total and it is divided in %lu pieces of %lu bytes each\n",
          tf->length, tf->num_pieces, tf->piece_length);
  tracker_response *tr = init_tracker_response();
  fprintf(stdout, "contacting tracker at %s\n", tf->announce);
  ok = contact_tracker(tr, tf);
  if (!ok) {
    fprintf(stderr, "contact_tracker failed\n");
    goto exit_1;
  }
  fprintf(stdout, "tracker replied with the addresses of %lu peers\n", tr->num_peers);

  pieces_pool *pp = init_pieces_pool(tf->num_pieces);
  size_t piece_index = get_piece_index(pp);
  for (size_t i = 0; i < tr->num_peers; i++) {
    fprintf(stdout, "\n");
    peer *p = tr->peers[i];
    char peer_addr[15 + 1];  // An address is of the form xxx.xxx.xxx.xxx + '\0'
    sprintf(peer_addr, "%d.%d.%d.%d", p->address[0], p->address[1], p->address[2], p->address[3]);
    conn *c = init_conn(peer_addr, p->port, TIMEOUT_SEC);
    if (c == NULL) {
      fprintf(stderr, "init_conn failed\n");
      continue;
    }

    /***** HANDSHAKE *****/
    bool handshake_ok = false;
    handshake_msg *hm = init_handshake_msg(tf->info_hash, MY_PEER_ID);
    handshake_msg_encoded *hm_encoded = encode_handshake_msg(hm);
    fprintf(stdout, "performing handshake with peer %s:%hu\n", peer_addr, p->port);
    ok = send_data(c, hm_encoded->buf, hm_encoded->size, TIMEOUT_SEC);
    if (!ok) {
      fprintf(stderr, "send_data failed\n");
      goto exit_handshake_0;
    }
    char *hm_buf = malloc(hm_encoded->size);
    ok = receive_data(c, hm_buf, hm_encoded->size, TIMEOUT_SEC);
    if (!ok) {
      fprintf(stderr, "receive_data failed\n");
      goto exit_handshake_1;
    }
    fprintf(stdout, "performed handshake with peer %s:%hu\n", peer_addr, p->port);
    handshake_msg *hm_reply = decode_handshake_msg(hm_buf, hm_encoded->size);
    if (hm_reply == NULL) {
      fprintf(stderr, "decode_handshake_msg failed\n");
      goto exit_handshake_1;
    }
    if (memcmp(hm_reply->info_hash, hm->info_hash, BT_HASH_LENGTH) != 0) {
      fprintf(stderr, "info hashes don't match, aborting connection with peer\n");
      goto exit_handshake_2;
    }
    handshake_ok = true;
  exit_handshake_2:
    free_handshake_msg(hm_reply);
  exit_handshake_1:
    free(hm_buf);
  exit_handshake_0:
    free_handshake_msg_encoded(hm_encoded);
    free_handshake_msg(hm);
    if (!handshake_ok) {
      free_conn(c);
      continue;
    }
    /**********************/

    // NOTE: the bitfield is optional, some peers could send "HAVE" messages.
    // For now, we assume that after the handshake, the bitfield message is sent.
    /***** BITFIELD ******/
    bool bitfield_ok = false;
    size_t bitfield_msg_size = ceil(tf->num_pieces / 8.0) + MSG_ID_BYTES + MSG_LEN_BYTES;
    char *bitfield_buf = malloc(bitfield_msg_size);
    fprintf(stdout, "receving bitfield from peer %s:%hu\n", peer_addr, p->port);
    ok = receive_data(c, bitfield_buf, bitfield_msg_size, TIMEOUT_SEC);
    if (!ok) {
      fprintf(stderr, "receive_data failed\n");
      goto bitfield_exit_0;
    }
    message *bitfield_msg = decode_message(bitfield_buf, bitfield_msg_size);
    if (bitfield_msg == NULL) {
      fprintf(stderr, "decode_message failed\n");
      goto bitfield_exit_1;
    }
    fprintf(stdout, "received bitifield from peer %s:%hu\n", peer_addr, p->port);
    bitfield *bf =
        init_bitfield((unsigned char *)bitfield_msg->payload, bitfield_msg->payload_len);

    bitfield_ok = true;
  bitfield_exit_1:
    free_message(bitfield_msg);
  bitfield_exit_0:
    free(bitfield_buf);
    if (!bitfield_ok) {
      free_conn(c);
      continue;
    }
    /*********************/

    if (!has_piece(bf, piece_index)) {
      fprintf(stdout, "peer %s:%hu doesn't have piece #%lu\n", peer_addr, p->port, piece_index);
      free_bitfield(bf);
      free_conn(c);
      continue;
    }
    fprintf(stdout, "peer %s:%hu has piece #%lu\n", peer_addr, p->port, piece_index);

    free_bitfield(bf);
    free_conn(c);
    break;
  }

  free_pieces_pool(pp);
exit_1:
  free_tracker_response(tr);
exit_0:
  free_torrent_file(tf);
  return ok;
}