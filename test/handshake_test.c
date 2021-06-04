#include "handshake_test.h"

#include <stdio.h>

#include "../src/handshake.h"
#include "../src/torrent_file.h"
#include "../src/tracker_response.h"

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

void test_perform_handshake()
{
  torrent_file *tf = init_torrent_file();
  int err = parse_torrent_file(tf, "test/data/debian-10.9.0-amd64-netinst.iso.torrent");
  if (err != TORRENT_OK) {
    fprintf(stderr, "Parsing failed: got error code %d\n", err);
    goto exit;
  }
  char peer_id[PEER_ID_LENGTH + 1] = "BTB-14011996ecis2211";
  tracker_response *r = init_tracker_response();
  int n = contact_tracker(r, tf, "mysuperduperpeeid___");
  if (n != 0) {
    fprintf(stderr, "failed to contact tracker\n");
    goto exit;
  }
  handshake_msg *h = init_handshake_msg(peer_id, tf->info_hash);
  for (long i = 0; i < r->num_peers; i++) {
    int peer_socket = perform_handshake(r->peers[i], h);
    if (peer_socket > 1) {
      break;
    }
  }
  free_handshake_msg(h);

exit:
  free_torrent_file(tf);
  free_tracker_response(r);
}
