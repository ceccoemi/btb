#include "handshake_test.h"

#include <stdio.h>

#include "../src/handshake.h"
#include "../src/torrent_file.h"
#include "../src/tracker_response.h"

void test_perform_handshake()
{
  torrent_file *tf = init_torrent_file();
  int err = parse_torrent_file(tf, "test/data/debian-10.9.0-amd64-netinst.iso.torrent");
  if (err != TORRENT_OK) {
    fprintf(stderr, "parsing failed: got error code %d\n", err);
    goto exit;
  }
  unsigned char peer_id[PEER_ID_LENGTH + 1] = "BTB-14011996ecis2211";
  tracker_response *r = contact_tracker(tf, peer_id);
  if (r == NULL) {
    fprintf(stderr, "failed to contact the tracker");
    goto exit;
  }
  err = perform_handshake(r, peer_id, tf->info_hash);

exit:
  free_torrent_file(tf);
  free_tracker_response(r);
}
