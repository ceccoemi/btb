#include "../src/torrent.h"

#include <stdio.h>
#include <string.h>

#include "torrent_test.h"

void test_torrent_file()
{
  torrent *t = init_torrent();
  int err = parse_torrent_file(
      t, "test/data/debian-10.9.0-amd64-netinst.iso.torrent");
  if (err != TORRENT_OK) {
    fprintf(stderr, "parsing failed: got error code %d\n", err);
    free_torrent(t);
    return;
  }
  char want_announce[] = "http://bttracker.debian.org:6969/announce";
  if (memcmp(t->announce, want_announce, t->announce_size)) {
    fprintf(stderr, "wrong announce: got %s, want %s\n", t->announce,
            want_announce);
    free_torrent(t);
    return;
  }
  free_torrent(t);
}
