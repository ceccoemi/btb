#include "client.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "big_endian.h"
#include "conn.h"
#include "defer.h"
#include "handshake_msg.h"
#include "message.h"
#include "peer.h"
#include "peer_id.h"
#include "piece_progress.h"
#include "pieces_pool.h"
#include "torrent_file.h"
#include "tracker_response.h"

#define TIMEOUT_SEC 5

bool download_torrent(const char *filename)
{
  torrent_file *tf = init_torrent_file();
  DEFER({ free_torrent_file(tf); });
  bool ok = parse_torrent_file(tf, filename);
  if (!ok) {
    fprintf(stderr, "parse_torrent_file failed\n");
    return false;
  }
  fprintf(stdout,
          "the file is %lu bytes total and it is divided in %lu pieces of %lu bytes each\n",
          tf->length, tf->num_pieces, tf->piece_length);
  tracker_response *tr = init_tracker_response();
  DEFER({ free_tracker_response(tr); });
  fprintf(stdout, "contacting tracker at %s\n", tf->announce);
  ok = contact_tracker(tr, tf);
  if (!ok) {
    fprintf(stderr, "contact_tracker failed\n");
    return false;
  }
  fprintf(stdout, "tracker replied with the addresses of %lu peers\n", tr->num_peers);

  pieces_pool *pp = init_pieces_pool(tf->num_pieces);
  DEFER({ free_pieces_pool(pp); });
  size_t piece_index = get_piece_index(pp);
  for (size_t i = 0; i < tr->num_peers; i++) {
    fprintf(stdout, "\n");
    peer *p = tr->peers[i];
    char peer_addr[15 + 1];  // An address is of the form xxx.xxx.xxx.xxx + '\0'
    sprintf(peer_addr, "%d.%d.%d.%d", p->address[0], p->address[1], p->address[2], p->address[3]);
    conn *c = init_conn(peer_addr, p->port, TIMEOUT_SEC);
    DEFER({ free_conn(c); });
    if (c == NULL) {
      fprintf(stderr, "init_conn failed\n");
      continue;
    }

    /***** HANDSHAKE *****/
    handshake_msg *hm = init_handshake_msg(tf->info_hash, MY_PEER_ID);
    DEFER({ free_handshake_msg(hm); });
    handshake_msg_encoded *hm_encoded = encode_handshake_msg(hm);
    DEFER({ free_handshake_msg_encoded(hm_encoded); });
    fprintf(stdout, "performing handshake with peer %s:%hu\n", peer_addr, p->port);
    ok = send_data(c, hm_encoded->buf, hm_encoded->size, TIMEOUT_SEC);
    if (!ok) {
      fprintf(stderr, "send_data failed\n");
      continue;
    }
    char *hm_buf = malloc(hm_encoded->size);
    DEFER({ free(hm_buf); });
    int bytes_receive = receive_data(c, hm_buf, hm_encoded->size, TIMEOUT_SEC);
    if (bytes_receive <= 0) {
      fprintf(stderr, "receive_data failed\n");
      continue;
    }
    fprintf(stdout, "performed handshake with peer %s:%hu\n", peer_addr, p->port);
    handshake_msg *hm_reply = decode_handshake_msg(hm_buf, hm_encoded->size);
    DEFER({ free_handshake_msg(hm_reply); });
    if (hm_reply == NULL) {
      fprintf(stderr, "decode_handshake_msg failed\n");
      continue;
    }
    if (memcmp(hm_reply->info_hash, hm->info_hash, BT_HASH_LENGTH) != 0) {
      fprintf(stderr, "info hashes don't match, aborting connection with peer\n");
      continue;
    }
    /**********************/

    // NOTE: the bitfield is optional, some peers could send "HAVE" messages.
    // For now, we assume that after the handshake, the bitfield message is sent.
    /***** BITFIELD ******/
    size_t bitfield_msg_size = ceil(tf->num_pieces / 8.0) + MSG_ID_BYTES + MSG_LEN_BYTES;
    char *bitfield_buf = malloc(bitfield_msg_size);
    DEFER({ free(bitfield_buf); });
    fprintf(stdout, "receving bitfield from peer %s:%hu\n", peer_addr, p->port);
    message *bitfield_msg = read_message_from_conn(c, TIMEOUT_SEC);
    if (bitfield_msg == NULL) {
      fprintf(stderr, "decode_message failed\n");
    }
    DEFER({ free_message(bitfield_msg); });
    fprintf(stdout, "received bitifield from peer %s:%hu\n", peer_addr, p->port);
    bitfield *bf =
        init_bitfield((unsigned char *)bitfield_msg->payload, bitfield_msg->payload_len);
    DEFER({ free_bitfield(bf); });
    /*********************/

    if (!has_piece(bf, piece_index)) {
      fprintf(stdout, "peer %s:%hu doesn't have piece #%lu\n", peer_addr, p->port, piece_index);
      continue;
    }
    fprintf(stdout, "peer %s:%hu has piece #%lu\n", peer_addr, p->port, piece_index);

    /**** INTERESTED *****/
    message *interested_msg = init_message(MSG_ID_INTERESTED, 0, NULL);
    DEFER({ free_message(interested_msg); });
    fprintf(stdout, "sending \"Interested\" message to peer %s:%hu\n", peer_addr, p->port);
    ok = send_message_to_conn(interested_msg, c, TIMEOUT_SEC);
    if (!ok) {
      fprintf(stderr, "send_data failed\n");
      continue;
    }
    fprintf(stdout, "sent \"Interested\" message to peer %s:%hu\n", peer_addr, p->port);
    /*********************/

    /******* State *******/
    piece_progress *p_prog = init_piece_progress(piece_index, tf->piece_length);
    DEFER({ free_piece_progress(p_prog); });
    bool next_peer = false;
    // while (true) {
    message *recv_msg = read_message_from_conn(c, TIMEOUT_SEC);
    if (recv_msg == NULL) {
      fprintf(stderr, "read_message_from_conn failed\n");
      break;
    }
    DEFER({ free_message(recv_msg); });
    switch (recv_msg->id) {
      case MSG_ID_CHOKE:
        fprintf(stdout, "received a \"Choke\" message\n");
        break;
      case MSG_ID_UNCHOKE:
        fprintf(stdout, "received a \"Unchoke\" message\n");
        unsigned char
            request_msg_payload[PIECE_INDEX_BYTES + BLOCK_OFFSET_BYTES + BLOCK_LENGTH_BYTES];
        ok = lu_to_big_endian(piece_index, request_msg_payload, PIECE_INDEX_BYTES);
        if (!ok) {
          fprintf(stderr, "lu_to_big_endian failed\n");
          goto next_peer;
        }
        // Block size of 2^14 = 16384 (TODO move to constant)
        size_t block_size = 16384;
        size_t block_offset = p_prog->downloaded * block_size;
        ok = lu_to_big_endian(block_offset, request_msg_payload + PIECE_INDEX_BYTES,
                              BLOCK_OFFSET_BYTES);
        if (!ok) {
          fprintf(stderr, "lu_to_big_endian failed\n");
          goto next_peer;
        }
        ok = lu_to_big_endian(block_size,
                              request_msg_payload + PIECE_INDEX_BYTES + BLOCK_OFFSET_BYTES,
                              BLOCK_LENGTH_BYTES);
        if (!ok) {
          fprintf(stderr, "lu_to_big_endian failed\n");
          goto next_peer;
        }

        message *msg_request = init_message(
            MSG_ID_REQUEST, PIECE_INDEX_BYTES + BLOCK_OFFSET_BYTES + BLOCK_LENGTH_BYTES,
            (char *)request_msg_payload);
        DEFER({ free_message(msg_request); });
        fprintf(stdout, "sending \"Request\" message for a block with offset %lu of piece #%lu\n",
                block_offset, piece_index);
        ok = send_message_to_conn(msg_request, c, TIMEOUT_SEC);
        if (!ok) {
          fprintf(stderr, "send_message_to_conn failed\n");
          goto next_peer;
        }
        fprintf(stdout, "\"Request\" message sent\n");

        break;
      case MSG_ID_INTERESTED:
        fprintf(stdout, "received a \"Interested\" message\n");
        break;
      case MSG_ID_REQUEST:
        fprintf(stdout, "received a \"Request\" message\n");
        break;
      case MSG_ID_PIECE:
        fprintf(stdout, "received a \"Piece\" message\n");
        break;
      case MSG_ID_CANCEL:
        fprintf(stdout, "received a \"Cancel\" message\n");
        break;
      default:
        fprintf(stdout, "Unknown or useless message ID: %hu\n", recv_msg->id);
        break;
    }
    //}
  next_peer:
    if (next_peer) {
      continue;
    }
    /*********************/

    break;
  }

  return true;
}