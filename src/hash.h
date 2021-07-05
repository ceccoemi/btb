#pragma once

#include <stdbool.h>
#include <stddef.h>

#define BT_HASH_LENGTH 20  // SHA1

// Perform the hash of the data in `in` and save the result in `out`.
// It returns true if it succeeds.
bool perform_hash(unsigned char *in, size_t size_in, unsigned char out[BT_HASH_LENGTH]);