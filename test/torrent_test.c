#include "../src/torrent.h"

#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

#include "torrent_test.h"

void test_debian_torrent()
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
  long long want_length = 353370112;
  if (t->length != want_length) {
    fprintf(stderr, "wrong length: got %lld, want %lld\n", t->length, want_length);
    goto exit;
  }
  long long want_piece_length = 262144;
  if (t->piece_length != want_piece_length) {
    fprintf(stderr, "wrong piece_length: got %lld, want %lld\n", t->piece_length,
            want_piece_length);
    goto exit;
  }
  long long want_num_pieces = 26960 / SHA_DIGEST_LENGTH;
  if (t->num_pieces != want_num_pieces) {
    fprintf(stderr, "wrong num_pieces: got %lld, want %lld\n", t->num_pieces, want_num_pieces);
    goto exit;
  }
  if (t->piece_hashes == NULL) {
    fprintf(stderr, "piece_hashes is NULL\n");
    goto exit;
  }
  for (long long i = 0; i < t->num_pieces; i++) {
    if (t->piece_hashes[i] == NULL) {
      fprintf(stderr, "%lld-th piece hash is NULL\n", i);
      goto exit;
    }
  }
  if (t->info_hash == NULL) {
    fprintf(stderr, "info_hash is NULL\n");
    goto exit;
  }
exit:
  free_torrent(t);
}
