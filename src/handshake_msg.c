#include "handshake_msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

handshake_msg *init_handshake_msg(const unsigned char info_hash[BT_HASH_LENGTH],
                                  const char peer_id[PEER_ID_LENGTH])
{
  handshake_msg *hm = malloc(sizeof(handshake_msg));
  hm->pstrlen = 19l;
  hm->pstr = malloc(hm->pstrlen + 1);
  strcpy(hm->pstr, "BitTorrent protocol");
  memset(hm->reserved, 0, HANDSHAKE_RESERVED_LEN);
  memcpy(hm->info_hash, info_hash, BT_HASH_LENGTH);
  memcpy(hm->peer_id, peer_id, PEER_ID_LENGTH);
  return hm;
}

void free_handshake_msg(handshake_msg *hm)
{
  if (hm == NULL) return;
  free(hm->pstr);
  free(hm);
}

handshake_msg_encoded *encode_handshake_msg(handshake_msg *hm)
{
  handshake_msg_encoded *encoded = malloc(sizeof(handshake_msg_encoded));
  encoded->size = 1 + hm->pstrlen + HANDSHAKE_RESERVED_LEN + BT_HASH_LENGTH + PEER_ID_LENGTH;
  encoded->buf = malloc(encoded->size);
  char *last = encoded->buf;
  last[0] = hm->pstrlen;
  last++;
  memcpy(last, hm->pstr, hm->pstrlen);
  last += hm->pstrlen;
  memcpy(last, hm->reserved, HANDSHAKE_RESERVED_LEN);
  last += HANDSHAKE_RESERVED_LEN;
  memcpy(last, hm->info_hash, BT_HASH_LENGTH);
  last += BT_HASH_LENGTH;
  memcpy(last, hm->peer_id, PEER_ID_LENGTH);
  last += PEER_ID_LENGTH;
  return encoded;
}

void free_handshake_msg_encoded(handshake_msg_encoded *encoded)
{
  if (encoded == NULL) return;
  free(encoded->buf);
  free(encoded);
}

handshake_msg *decode_handshake_msg(char *buf, size_t buf_length)
{
  if (buf_length == 0) {
    fprintf(stderr, "too small buffer to decode handshake_msg: got %lu, want > 0\n", buf_length);
    return NULL;
  }
  handshake_msg *hm = malloc(sizeof(handshake_msg));
  char *last = buf;
  hm->pstrlen = last[0];
  last++;
  if (buf_length < last - buf + hm->pstrlen) {
    free(hm);
    fprintf(stderr, "too small buffer to decode handshake_msg: got %lu, want %ld\n", buf_length,
            last - buf + hm->pstrlen);
    return NULL;
  }
  hm->pstr = malloc(hm->pstrlen + 1);
  strncpy(hm->pstr, last, hm->pstrlen);
  hm->pstr[hm->pstrlen] = '\0';
  last += hm->pstrlen;
  if (buf_length < last - buf + HANDSHAKE_RESERVED_LEN) {
    free(hm->pstr);
    free(hm);
    fprintf(stderr, "too small buffer to decode handshake_msg: got %lu, want %ld\n", buf_length,
            last - buf + HANDSHAKE_RESERVED_LEN);
    return NULL;
  }
  memcpy(hm->reserved, last, HANDSHAKE_RESERVED_LEN);
  last += HANDSHAKE_RESERVED_LEN;
  if (buf_length < last - buf + BT_HASH_LENGTH) {
    free(hm->pstr);
    free(hm);
    fprintf(stderr, "too small buffer to decode handshake_msg: got %lu, want %ld\n", buf_length,
            last - buf + BT_HASH_LENGTH);
    return NULL;
  }
  memcpy(hm->info_hash, last, BT_HASH_LENGTH);
  last += BT_HASH_LENGTH;
  if (buf_length < last - buf + PEER_ID_LENGTH) {
    free(hm->pstr);
    free(hm);
    fprintf(stderr, "too small buffer to decode handshake_msg: got %lu, want %ld\n", buf_length,
            last - buf + PEER_ID_LENGTH);
    return NULL;
  }
  memcpy(hm->peer_id, last, PEER_ID_LENGTH);
  return hm;
}
