#include "handshake_msg_test.h"

#include <stdio.h>
#include <string.h>

#include "../src/handshake_msg.h"
#include "../src/hash.h"
#include "../src/peer_id.h"

void test_handshake_msg()
{
  unsigned char info_hash[BT_HASH_LENGTH] = "1234567890abcdefgjkh";
  handshake_msg* hm = init_handshake_msg(info_hash, MY_PEER_ID);
  if (hm == NULL) {
    fprintf(stderr, "got NULL handshake_msg\n");
    goto exit;
  }
  if (hm->pstrlen != 19) {
    fprintf(stderr, "got wrong pstrlen: got %lu, want %lu\n", hm->pstrlen, 19l);
  }
  if (strcmp(hm->pstr, "BitTorrent protocol") != 0) {
    fprintf(stderr, "got wrong pstr: got %s, want %s\n", hm->pstr, "BitTorrent protocol");
  }
  char want_reserved[HANDSHAKE_RESERVED_LEN] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (memcmp(hm->reserved, want_reserved, HANDSHAKE_RESERVED_LEN) != 0) {
    fprintf(stderr, "wrong reserved: got %*.s, want %*.s\n", HANDSHAKE_RESERVED_LEN, hm->reserved,
            HANDSHAKE_RESERVED_LEN, want_reserved);
  }
  if (memcmp(info_hash, hm->info_hash, BT_HASH_LENGTH) != 0) {
    fprintf(stderr, "wrong info_hash: got %*.s, want %*.s\n", BT_HASH_LENGTH, hm->info_hash,
            BT_HASH_LENGTH, info_hash);
  }
  if (memcmp(MY_PEER_ID, hm->peer_id, PEER_ID_LENGTH) != 0) {
    fprintf(stderr, "wrong peer_id: got %*.s, want %*.s\n", PEER_ID_LENGTH, hm->peer_id,
            PEER_ID_LENGTH, MY_PEER_ID);
  }

exit:
  free_handshake_msg(hm);
}

void test_encoding_decoding_handshake_msg()
{
  unsigned char info_hash[BT_HASH_LENGTH] = "1234567890abcdefgjkh";
  handshake_msg* hm = init_handshake_msg(info_hash, MY_PEER_ID);
  if (hm == NULL) {
    fprintf(stderr, "got NULL handshake_msg\n");
    goto exit;
  }

  char buf[128];
  int bytes_written = encode_handshake_msg(hm, buf);
  if (bytes_written < 0) {
    fprintf(stderr, "encode_handshake_msg failed\n");
    goto exit;
  }
  int want_bytes_written =
      1 + hm->pstrlen + HANDSHAKE_RESERVED_LEN + BT_HASH_LENGTH + PEER_ID_LENGTH;
  if (bytes_written != want_bytes_written) {
    fprintf(stderr, "wrong encoded bytes: got %d, want %d", bytes_written, want_bytes_written);
  }

  handshake_msg* decoded_hm = decode_handshake_msg(buf, bytes_written);
  if (decoded_hm == NULL) {
    fprintf(stderr, "decode_handshake_msg failed\n");
    goto exit;
  }
  if (memcmp(decoded_hm->info_hash, hm->info_hash, BT_HASH_LENGTH) != 0) {
    fprintf(stderr, "wrong decoded info_hash: got %*.s, want %*.s", BT_HASH_LENGTH,
            decoded_hm->info_hash, BT_HASH_LENGTH, hm->info_hash);
  }
  free_handshake_msg(decoded_hm);

exit:
  free_handshake_msg(hm);
}