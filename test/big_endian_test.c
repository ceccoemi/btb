#include "big_endian_test.h"

#include <stdio.h>
#include <stdlib.h>

#include "../src/big_endian.h"

void test_lu_to_big_endian_4_bytes()
{
  size_t buf_size = 4;
  unsigned char *buf = malloc(buf_size);
  unsigned int n = 1234;  // 00000000 00000000 00000100 11010010
  bool ok = lu_to_big_endian(n, buf, buf_size);
  if (!ok) {
    fprintf(stderr, "lu_to_big_endian failed\n");
    free(buf);
    return;
  }
  unsigned char want[buf_size];
  want[0] = 0;
  want[1] = 0;
  want[2] = 4;
  want[3] = 210;
  if (buf[0] != want[0] || buf[1] != want[1] || buf[2] != want[2] || buf[3] != want[3]) {
    fprintf(stderr, "lu_to_big_endian: got %u %u %u %u, want %u %u %u %u\n", buf[0], buf[1],
            buf[2], buf[3], want[0], want[1], want[2], want[3]);
  }
  free(buf);
}

void test_lu_to_big_endian_2_bytes()
{
  size_t buf_size = 2;
  unsigned char *buf = malloc(buf_size);
  unsigned int n = 302;  // 00000001 00101110
  bool ok = lu_to_big_endian(n, buf, buf_size);
  if (!ok) {
    fprintf(stderr, "lu_to_big_endian failed\n");
    free(buf);
    return;
  }
  unsigned char want[buf_size];
  want[0] = 1;
  want[1] = 46;
  if (buf[0] != want[0] || buf[1] != want[1]) {
    fprintf(stderr, "lu_to_big_endian: got %u %u, want %u %u\n", buf[0], buf[1], want[0], want[1]);
  }
  free(buf);
}

void test_lu_to_big_endian_1_bytes()
{
  size_t buf_size = 1;
  unsigned char *buf = malloc(buf_size);
  unsigned int n = 65;  // 01000001
  bool ok = lu_to_big_endian(n, buf, buf_size);
  if (!ok) {
    fprintf(stderr, "lu_to_big_endian failed\n");
    free(buf);
    return;
  }
  unsigned char want[buf_size];
  want[0] = 65;
  if (buf[0] != want[0]) {
    fprintf(stderr, "lu_to_big_endian: got %u, want %u\n", buf[0], want[0]);
  }
  free(buf);
}
