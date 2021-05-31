#include "worker.h"

#include <stddef.h>
#include <stdio.h>

#include "bitfield.h"
#include "handshake.h"
#include "message.h"
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
  size_t pieces_downloaded = 0;
  for (long i = 0; i < r->num_peers; i++) {
    handshake_msg *h = init_handshake_msg(peer_id, tf->info_hash);
    int peer_socket = perform_handshake(r->peers[i], h);
    free_handshake_msg(h);
    if (peer_socket < 0) {
      fprintf(stderr, "failed to perform handshake\n");
      continue;
    }

    fprintf(stdout, "Reading bitfield\n");
    message *msg = read_message(peer_socket);
    if (msg == NULL) {
      continue;
    }
    bitfield *peer_bitfield = NULL;
    if (msg->id == MSG_BITFIELD) {
      fprintf(stdout, "Received bitfield\n");
      peer_bitfield = init_bitfield(msg->payload, msg->payload_len);
      free_message(msg);
    } else {
      fprintf(stdout, "received message with id %hu, skipping...\n", msg->id);
      continue;
    }

    fprintf(stdout, "Sending interested message\n");
    message *interested_msg = create_message(MSG_INTERESTED, 0, NULL);
    if (send_message(peer_socket, interested_msg) < 0) {
      fprintf(stderr, "error in sending interested message\n");
      free_message(interested_msg);
      break;
    }
    fprintf(stdout, "Sent interested message\n");
    free_message(interested_msg);

    fprintf(stdout, "Reading message\n");
    message *msg_2 = read_message(peer_socket);
    if (msg_2 != NULL) {
      fprintf(stdout, "Received message %hu\n", msg_2->id);
      free_message(msg_2);
    }
    break;
  }

exit:
  free_torrent_file(tf);
  free_tracker_response(r);

  return pieces_downloaded;
}
