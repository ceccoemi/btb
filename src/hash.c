#include "hash.h"

#include <openssl/sha.h>
#include <stdio.h>

bool perform_hash(unsigned char *in, size_t size_in, unsigned char out[BT_HASH_LENGTH])
{
  if (BT_HASH_LENGTH != SHA_DIGEST_LENGTH) {
    fprintf(stderr, "BT_HASH_LENGTH should be %d\n", SHA_DIGEST_LENGTH);
    return false;
  }
  SHA1(in, size_in, out);
  return true;
}