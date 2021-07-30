#include "pieces_pool.h"

#include <stdlib.h>

pieces_pool* init_pieces_pool(size_t num_pieces)
{
  pieces_pool* pool = malloc(sizeof(pieces_pool));
  pool->num_pieces = num_pieces;
  unsigned char* bitfield_data = calloc(1, pool->num_pieces);
  pool->current_bitfield = init_bitfield(bitfield_data, pool->num_pieces);
  free(bitfield_data);
  return pool;
}

size_t get_piece_index(pieces_pool* pool)
{
  for (size_t i = 0; i < pool->num_pieces; i++) {
    if (!has_piece(pool->current_bitfield, i)) {
      set_piece(pool->current_bitfield, i);
      return i;
    }
  }
  return pool->num_pieces + 1;
}

void mark_as_undone(pieces_pool* pool, size_t index)
{
  unset_piece(pool->current_bitfield, index);
}

bool is_done(pieces_pool* pool)
{
  size_t pieces_sum = 0;
  for (size_t i = 0; i < pool->num_pieces; i++) {
    if (has_piece(pool->current_bitfield, i)) {
      pieces_sum++;
    }
  }
  return pieces_sum == pool->num_pieces;
}

size_t get_num_undone_pieces(pieces_pool* pool)
{
  size_t pieces_sum = 0;
  for (size_t i = 0; i < pool->num_pieces; i++) {
    if (has_piece(pool->current_bitfield, i)) {
      pieces_sum++;
    }
  }
  return pool->num_pieces - pieces_sum;
}

void free_pieces_pool(pieces_pool* pool)
{
  if (pool == NULL) return;
  free_bitfield(pool->current_bitfield);
  free(pool);
}
