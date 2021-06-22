#include "big_endian.h"

#include <stdio.h>

bool lu_to_big_endian(unsigned long n, unsigned char* buf, size_t buf_size)
{
  for (size_t i = 0; i < buf_size; i++) {
    buf[i] = n >> (8 * (buf_size - i - 1));
  }
  return true;
}

unsigned long big_endian_to_lu(unsigned char* buf, size_t buf_size)
{
  unsigned long n = 0;
  for (size_t i = 0; i < buf_size; i++) {
    n += buf[i] << (8 * (buf_size - i - 1));
  }
  return n;
}
