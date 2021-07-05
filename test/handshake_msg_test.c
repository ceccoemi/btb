#include "handshake_msg_test.h"

#include <stdio.h>
#include <string.h>

#include "../src/handshake_msg.h"
#include "../src/hash.h"
#include "../src/peer_id.h"

void test_handshake_msg()
{
  unsigned char info_hash[BT_HASH_LENGTH] = "1234567890abcdefgjkh";
  char my_peer_id[PEER_ID_LENGTH] = "MY-PEER-ID0123456789";
  handshake_msg* hm = init_handshake_msg(info_hash, my_peer_id);
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

exit:
  free_handshake_msg(hm);
}