#include "pieces_queue.h"

#include <stdlib.h>

pieces_queue* init_pieces_queue(size_t num_pieces)
{
  pieces_queue* q = malloc(sizeof(pieces_queue));
  q->num_pieces = num_pieces;
  unsigned char* bitfield_data = calloc(1, q->num_pieces);
  q->current_bitfield = init_bitfield(bitfield_data, q->num_pieces);
  free(bitfield_data);
  return q;
}

size_t get_piece_index(pieces_queue* q)
{
  for (size_t i = 0; i < q->num_pieces; i++) {
    if (!has_piece(q->current_bitfield, i)) {
      return i;
    }
  }
  return q->num_pieces + 1;
}

void mark_as_done(pieces_queue* q, size_t index) { set_piece(q->current_bitfield, index); }

bool is_done(pieces_queue* q)
{
  size_t pieces_sum = 0;
  for (size_t i = 0; i < q->num_pieces; i++) {
    if (has_piece(q->current_bitfield, i)) {
      pieces_sum++;
    }
  }
  return pieces_sum == q->num_pieces;
}

void free_pieces_queue(pieces_queue* q)
{
  free_bitfield(q->current_bitfield);
  free(q);
}
