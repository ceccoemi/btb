#include "pieces_pool.h"

#include <stdlib.h>

pieces_pool* init_pieces_pool(size_t num_pieces)
{
  pieces_pool* q = malloc(sizeof(pieces_pool));
  q->num_pieces = num_pieces;
  unsigned char* bitfield_data = calloc(1, q->num_pieces);
  q->current_bitfield = init_bitfield(bitfield_data, q->num_pieces);
  free(bitfield_data);
  return q;
}

size_t get_piece_index(pieces_pool* q)
{
  for (size_t i = 0; i < q->num_pieces; i++) {
    if (!has_piece(q->current_bitfield, i)) {
      set_piece(q->current_bitfield, i);
      return i;
    }
  }
  return q->num_pieces + 1;
}

void mark_as_undone(pieces_pool* q, size_t index) { unset_piece(q->current_bitfield, index); }

bool is_done(pieces_pool* q)
{
  size_t pieces_sum = 0;
  for (size_t i = 0; i < q->num_pieces; i++) {
    if (has_piece(q->current_bitfield, i)) {
      pieces_sum++;
    }
  }
  return pieces_sum == q->num_pieces;
}

void free_pieces_pool(pieces_pool* q)
{
  if (q == NULL) return;
  free_bitfield(q->current_bitfield);
  free(q);
}
