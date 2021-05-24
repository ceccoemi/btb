#include "worker_test.h"

#include <stdio.h>

#include "../src/bitfield.h"
#include "../src/handshake.h"
#include "../src/message.h"
#include "../src/pieces_queue.h"
#include "../src/torrent_file.h"
#include "../src/tracker_response.h"

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

void test_worker()
{
  torrent_file *tf = init_torrent_file();
  int err = parse_torrent_file(tf, "test/data/debian-10.9.0-amd64-netinst.iso.torrent");
  if (err != TORRENT_OK) {
    fprintf(stderr, "parsing failed: got error code %d\n", err);
    goto exit;
  }
  char peer_id[PEER_ID_LENGTH + 1] = "BTB-14011996ecis2211";
  tracker_response *r = contact_tracker(tf, peer_id);
  if (r == NULL) {
    fprintf(stderr, "failed to contact the tracker");
    goto exit;
  }
  handshake_msg *h = init_handshake_msg(peer_id, tf->info_hash);
  for (long i = 0; i < r->num_peers; i++) {
    int sockfd = perform_handshake(r->peers[i], h);
    if (sockfd < 0) continue;
    bitfield *peer_bitfield = NULL;
    for (int j = 0; j < 10; j++) {
      message *msg = read_message(sockfd);
      if (msg->id == MSG_BITFIELD) {
        fprintf(stdout, "Received bitfield\n");
        peer_bitfield = init_bitfield(msg->payload, msg->payload_len);
        free_message(msg);
        break;
      }
      free_message(msg);
      fprintf(stdout, "msg id: %hu\n", msg->id);
    }
    if (peer_bitfield == NULL) {
      fprintf(stderr, "not able to receive a bitfield\n");
      goto exit;
    }
    pieces_queue *q = init_pieces_queue(tf->num_pieces);
    break;
  }

exit:
  free_torrent_file(tf);
  free_tracker_response(r);
  free_handshake_msg(h);
}
