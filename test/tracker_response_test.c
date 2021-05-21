#include "tracker_response_test.h"

#include <stdio.h>

#include "../src/torrent_file.h"
#include "../src/tracker_response.h"

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

void test_contact_tracker()
{
  torrent_file *tf = init_torrent_file();
  int err = parse_torrent_file(tf, "test/data/debian-10.9.0-amd64-netinst.iso.torrent");
  if (err != TORRENT_OK) {
    fprintf(stderr, "parsing failed: got error code %d\n", err);
    goto exit;
  }
  tracker_response *r = contact_tracker(tf, "mysuperduperpeeid___");
  if (r == NULL) {
    fprintf(stderr, "failed to contact tracker\n");
    goto exit;
  }
  if (r->interval != 900l) {
    fprintf(stderr, "wrong interval: got %ld, want %ld\n", r->interval, 900l);
    goto exit;
  }
  for (long i = 0; i < r->num_peers; i++) {
    if (r->peers[i]->port == 0) {
      fprintf(stderr, "peer with unexpected 0 port\n");
      goto exit;
    }
    if (r->peers[i]->address == NULL) {
      fprintf(stderr, "i-%ld peer has a NULL address\n", i);
      goto exit;
    }
  }
  if (r->num_peers < 0) {
    fprintf(stderr, "negative number of peers\n");
    goto exit;
  }
exit:
  free_torrent_file(tf);
  free_tracker_response(r);
}
