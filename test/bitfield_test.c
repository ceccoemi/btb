#include "bitfield_test.h"

#include <stdio.h>

#include "../src/bitfield.h"
#include "../src/defer.h"
#include "../src/message.h"
#include "../src/torrent_file.h"
#include "../src/tracker_response.h"

void test_bitfield()
{
  unsigned char data[2] = {10, 33};  // 00001010 00100001
  bitfield *b = init_bitfield(data, 2);
  DEFER({ free_bitfield(b); });
  for (int i = 0; i < 4; i++) {
    if (has_piece(b, i)) {
      fprintf(stderr, "expected not having %d-th piece\n", i);
    }
  }
  if (!has_piece(b, 4)) {
    fprintf(stderr, "should have %d-th piece\n", 4);
  }
  if (has_piece(b, 5)) {
    fprintf(stderr, "expected not having %d-th piece\n", 5);
  }
  if (!has_piece(b, 6)) {
    fprintf(stderr, "should have %d-th piece\n", 6);
  }
  if (has_piece(b, 7)) {
    fprintf(stderr, "expected not having %d-th piece\n", 7);
  }
  for (int i = 8; i < 10; i++) {
    if (has_piece(b, i)) {
      fprintf(stderr, "expected not having %d-th piece\n", i);
    }
  }
  if (!has_piece(b, 10)) {
    fprintf(stderr, "should have %d-th piece\n", 10);
  }
  if (has_piece(b, 12)) {
    fprintf(stderr, "expected not having %d-th piece\n", 12);
  }
  if (!has_piece(b, 15)) {
    fprintf(stderr, "should have %d-th piece\n", 15);
  }

  set_piece(b, 2);
  if (!has_piece(b, 2)) {
    fprintf(stderr, "should have %d-th piece\n", 2);
  }
  set_piece(b, 12);
  if (!has_piece(b, 12)) {
    fprintf(stderr, "should have %d-th piece\n", 12);
  }

  unset_piece(b, 12);
  if (has_piece(b, 12)) {
    fprintf(stderr, "expected not having %d-th piece\n", 12);
  }
  unset_piece(b, 12);
  if (has_piece(b, 12)) {
    fprintf(stderr, "expected not having %d-th piece\n", 12);
  }

  set_piece(b, 12);
  if (!has_piece(b, 12)) {
    fprintf(stderr, "should have %d-th piece\n", 12);
  }
  set_piece(b, 12);
  if (!has_piece(b, 12)) {
    fprintf(stderr, "should have %d-th piece\n", 12);
  }

  // free_bitfield(b);
}
