#include "worker.h"

size_t download_pieces(torrent_file *tf)
{
  char peer_id[PEER_ID_LENGTH + 1] = "BTB-14011996ecis2211";
  tracker_response *r = contact_tracker(tf, peer_id);
  if (r == NULL) {
    fprintf(stderr, "failed to contact the tracker");
    goto exit;
  }
  handshake_msg *h = init_handshake_msg(peer_id, tf->info_hash);
  for (long i = 0; i < r->num_peers; i++) {
    int sockfd = perform_handshake(r->peers[i], h);
    if (sockfd < 0) {
      fprintf(stderr, "failed to perform handshake\n");
      continue;
    }
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
