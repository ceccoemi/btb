#include "worker.h"

#include <stddef.h>
#include <stdio.h>

#include "bitfield.h"
#include "handshake.h"
#include "message.h"
#include "pieces_queue.h"
#include "torrent_file.h"
#include "tracker_response.h"

size_t download_pieces(torrent_file *tf)
{
  char peer_id[PEER_ID_LENGTH + 1] = "BTB-14011996ecis2211";
  tracker_response *r = contact_tracker(tf, peer_id);
  if (r == NULL) {
    fprintf(stderr, "failed to contact the tracker");
    goto exit;
  }
  handshake_msg *h = init_handshake_msg(peer_id, tf->info_hash);
  size_t pieces_downloaded = 0;
  for (long i = 0; i < r->num_peers; i++) {
    int peer_socket = perform_handshake(r->peers[i], h);
    if (peer_socket < 0) {
      fprintf(stderr, "failed to perform handshake\n");
      continue;
    }
    bitfield *peer_bitfield = NULL;
    for (int j = 0; j < 10; j++) {
      message *msg = read_message(peer_socket);
      if (msg == NULL) {
        continue;
      }
      if (msg->id == MSG_BITFIELD) {
        fprintf(stdout, "Received bitfield\n");
        peer_bitfield = init_bitfield(msg->payload, msg->payload_len);
        free_message(msg);
        break;
      } else {
        fprintf(stdout, "received message with id %hu, skipping...\n", msg->id);
      }
      free_message(msg);
    }
    if (peer_bitfield == NULL) {
      fprintf(stderr, "not able to receive a bitfield\n");
      goto exit;
    }
    message *interested_msg = create_message(MSG_INTERESTED, 0, NULL);
    if (send_message(peer_socket, interested_msg) < 0) {
      fprintf(stderr, "error in sending interested message\n");
      free_message(interested_msg);
      break;
    }
    free_message(interested_msg);
    pieces_queue *q = init_pieces_queue(tf->num_pieces);
    break;
  }

exit:
  free_torrent_file(tf);
  free_tracker_response(r);
  free_handshake_msg(h);

  return pieces_downloaded;
}
