#include "bitfield_test.h"

#include <stdio.h>

#include "../src/bitfield.h"
#include "../src/handshake.h"
#include "../src/message.h"
#include "../src/torrent_file.h"
#include "../src/tracker_response.h"

void test_bitfield()
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
  for (long i = 0; i < r->num_peers && i < 10; i++) {
    int sockfd = perform_handshake(r->peers[i], h);
    if (sockfd < 0) continue;
    message *m = read_message(sockfd);
    if (m == NULL) continue;
    if (m->id == MSG_BITFIELD) {
      bitfield *b = init_bitfield(m->payload, m->payload_len);
      for (long long k = 0; k < tf->num_pieces; k += 8) {
        fprintf(stdout, "bitfield byte value: %i\n", b->data[k]);
        for (int j = k; j < k + 8; j++) {
          if (has_piece(b, j)) {
            fprintf(stdout, "has piece %d\n", j);
          }
        }
      }
      free_bitfield(b);
      break;
    }
  }
  free_handshake_msg(h);

exit:
  free_torrent_file(tf);
  free_tracker_response(r);
}