#include "../src/torrent_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/hash.h"
#include "torrent_file_test.h"

void test_sample_torrent()
{
  torrent_file *t = init_torrent_file();
  bool ok = parse_torrent_file(t, "test/data/sample.torrent");
  if (!ok) {
    fprintf(stderr, "parsing failed, expected success\n");
  }
  long long want_num_pieces = 3;
  if (t->num_pieces != want_num_pieces) {
    fprintf(stderr, "wrong num_pieces: got %lld, want %lld\n", t->num_pieces, want_num_pieces);
  }
  unsigned char want_piece_hashes[3][BT_HASH_LENGTH] = {
      {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
       'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
      {'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
       'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b'},
      {'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c',
       'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c', 'c'}};
  for (long long i = 0; i < want_num_pieces; i++) {
    if (memcmp(t->piece_hashes[i], want_piece_hashes[i], BT_HASH_LENGTH) != 0) {
      fprintf(stderr, "wrong %lld-th piece hash\n", i);
    }
  }
  char *info_dict =
      "d6:lengthi20e4:name10:sample.txt12:piece "
      "lengthi65536e6:pieces60:aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbcccccccccccccccccccc7:"
      "privatei1ee";
  unsigned char *want_hash_info = malloc(BT_HASH_LENGTH);
  perform_hash((unsigned char *)info_dict, strlen(info_dict), want_hash_info);
  if (memcmp(t->info_hash, want_hash_info, BT_HASH_LENGTH) != 0) {
    fprintf(stderr, "wrong info_hash\n");
  }
  free(want_hash_info);
  free_torrent_file(t);
}

void test_debian_torrent()
{
  torrent_file *t = init_torrent_file();
  bool ok = parse_torrent_file(t, "test/data/debian-10.9.0-amd64-netinst.iso.torrent");
  if (!ok) {
    fprintf(stderr, "parsing failed, expected success\n");
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
  long long want_num_pieces = 26960 / BT_HASH_LENGTH;
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
  free_torrent_file(t);
}
