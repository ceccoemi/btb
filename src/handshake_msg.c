#include "handshake_msg.h"

#include <stdlib.h>
#include <string.h>

handshake_msg *init_handshake_msg(unsigned char info_hash[BT_HASH_LENGTH],
                                  char peer_id[PEER_ID_LENGTH])
{
  handshake_msg *hm = malloc(sizeof(handshake_msg));
  hm->pstrlen = 19l;
  hm->pstr = malloc(hm->pstrlen + 1);
  // hm->pstr = "BitTorrent protocol";
  strcpy(hm->pstr, "BitTorrent protocol");
  return hm;
}

void free_handshake_msg(handshake_msg *hm)
{
  if (hm == NULL) return;
  free(hm->pstr);
  free(hm);
}
