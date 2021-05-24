#include "pieces_queue_test.h"

#include <stdio.h>

#include "../src/pieces_queue.h"

void test_pieces_queue()
{
  size_t num_pieces = 10;
  pieces_queue *q = init_pieces_queue(num_pieces);
  if (is_done(q)) {
    fprintf(stderr, "is_done should return false\n");
    goto exit;
  }
  size_t idx = get_piece_index(q);
  if (idx >= num_pieces) {
    fprintf(stderr, "wrong index: got %li, want x < %li\n", idx, num_pieces);
    goto exit;
  }
  mark_as_done(q, 0);
  for (size_t i = 0; i < num_pieces; i++) {
    idx = get_piece_index(q);
    if (idx >= num_pieces || idx == 0) {
      fprintf(stderr, "wrong index: got %li, want 0 < x < %li\n", idx, num_pieces);
      goto exit;
    }
  }
  if (is_done(q)) {
    fprintf(stderr, "is_done should return false\n");
    goto exit;
  }
  mark_as_done(q, 5);
  for (size_t i = 0; i < num_pieces; i++) {
    idx = get_piece_index(q);
    if (idx >= num_pieces || idx == 0 || idx == 5) {
      fprintf(stderr, "wrong index: got %li, want 0 < x < %li and x != 5\n", idx, num_pieces);
      goto exit;
    }
  }
  if (is_done(q)) {
    fprintf(stderr, "is_done should return false\n");
    goto exit;
  }
  for (size_t i = 0; i < num_pieces; i++) {
    mark_as_done(q, i);
  }
  for (size_t i = 0; i < num_pieces; i++) {
    idx = get_piece_index(q);
    if (idx < num_pieces) {
      fprintf(stderr, "wrong index (%li): all pieces should be done\n", idx);
      goto exit;
    }
  }
  if (!is_done(q)) {
    fprintf(stderr, "is_done should return true\n");
    goto exit;
  }

exit:
  free_pieces_queue(q);
}
