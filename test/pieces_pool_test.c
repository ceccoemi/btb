#include "pieces_pool_test.h"

#include <stdio.h>

#include "../src/defer.h"
#include "../src/pieces_pool.h"

void test_pieces_pool()
{
  size_t num_pieces = 10;
  pieces_pool *q = init_pieces_pool(num_pieces);
  DEFER({ free_pieces_pool(q); });
  if (is_done(q)) {
    fprintf(stderr, "is_done should return false\n");
  }
  size_t idx = get_piece_index(q);
  if (idx >= num_pieces) {
    fprintf(stderr, "wrong index: got %li, want x < %li\n", idx, num_pieces);
  }
  // Extract all pieces
  for (size_t i = 0; i < num_pieces - 1; i++) {
    idx = get_piece_index(q);
    if (idx >= num_pieces || idx == 0) {
      fprintf(stderr, "wrong index: got %li, want 0 < x < %li\n", idx, num_pieces);
    }
  }
  if (!is_done(q)) {
    fprintf(stderr, "is_done should return true\n");
  }
  idx = get_piece_index(q);
  if (idx < num_pieces) {
    fprintf(stderr, "get_piece_index should return >= num_pieces\n");
  }
  mark_as_undone(q, 0);
  if (is_done(q)) {
    fprintf(stderr, "is_done should return false\n");
  }
  idx = get_piece_index(q);
  if (idx != 0) {
    fprintf(stderr, "wrong index: got %li, want 0\n", idx);
  }
}
