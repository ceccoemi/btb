#include "protocol_test.h"

#include <stdio.h>

#include "../src/protocol.h"
#include "../src/torrent_file.h"

void test_contact_tracker()
{
  torrent_file *tf = init_torrent_file();
  int err = parse_torrent_file(tf, "test/data/debian-10.9.0-amd64-netinst.iso.torrent");
  if (err != TORRENT_OK) {
    fprintf(stderr, "parsing failed: got error code %d\n", err);
    goto exit;
  }
  tracker_response *r = contact_tracker(tf, "1111111111111111111");
exit:
  free_torrent_file(tf);
}
