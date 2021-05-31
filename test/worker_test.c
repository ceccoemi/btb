#include "worker_test.h"

#include <stdio.h>

#include "../src/worker.h"

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

void test_worker()
{
  torrent_file *tf = init_torrent_file();
  int err = parse_torrent_file(tf, "test/data/debian-10.9.0-amd64-netinst.iso.torrent");
  if (err != TORRENT_OK) {
    fprintf(stderr, "parsing failed: got error code %d\n", err);
    goto exit;
  }
  size_t pieces_dowloaded = download_pieces(tf);

exit:
  free_torrent_file(tf);
}
