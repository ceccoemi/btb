#include "peer_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/peer.h"

void test_peer()
{
  char peer_repr[PEER_BLOB_SIZE] = {124, 0, 100, 105, 1, 1};
  peer *p = init_peer(peer_repr);
  char wantAddr[] = "124.0.100.105";
  char *gotAddr = malloc(strlen(wantAddr) + 1);
  sprintf(gotAddr, "%d.%d.%d.%d", p->address[0], p->address[1], p->address[2], p->address[3]);
  if (strcmp(gotAddr, wantAddr) != 0) {
    fprintf(stderr, "wrong peer address: got %s, want %s\n", gotAddr, wantAddr);
    goto exit;
  }
  if (p->port != 257) {
    fprintf(stderr, "wrong peer port: got %d, want %d\n", p->port, 257);
    goto exit;
  }
exit:
  free(gotAddr);
  free_peer(p);
}
