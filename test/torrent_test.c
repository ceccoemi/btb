#include "../src/torrent.h"

#include <stdio.h>
#include <string.h>

#include "torrent_test.h"

void test_torrent_file()
{
  torrent *t = init_torrent();
  int err = parse_torrent_file(t, "test/data/debian-10.9.0-amd64-netinst.iso.torrent");
  if (err != TORRENT_OK) {
    fprintf(stderr, "parsing failed: got error code %d\n", err);
    goto exit;
  }
  char want_announce[] = "http://bttracker.debian.org:6969/announce";
  if (strcmp(t->announce, want_announce)) {
    fprintf(stderr, "wrong announce: got %s, want %s\n", t->announce, want_announce);
    goto exit;
  }
  char want_comment[] = "\"Debian CD from cdimage.debian.org\"";
  if (strcmp(t->comment, want_comment)) {
    fprintf(stderr, "wrong comment: got %s, want %s\n", t->comment, want_comment);
    goto exit;
  }
  char want_name[] = "debian-10.9.0-amd64-netinst.iso";
  if (strcmp(t->name, want_name)) {
    fprintf(stderr, "wrong name: got %s, want %s\n", t->name, want_name);
    goto exit;
  }
exit:
  free_torrent(t);
}
