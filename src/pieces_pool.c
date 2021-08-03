#include "pieces_pool.h"

#include <stdlib.h>

pieces_pool* init_pieces_pool(size_t num_pieces)
{
  pieces_pool* pool = malloc(sizeof(pieces_pool));
  pool->num_pieces = num_pieces;
  unsigned char* bitfield_data = calloc(1, pool->num_pieces);
  pool->current_bitfield = init_bitfield(bitfield_data, pool->num_pieces);
  free(bitfield_data);
  pthread_mutex_init(&pool->_mutex, NULL);
  return pool;
}

size_t get_piece_index(pieces_pool* pool)
{
  pthread_mutex_lock(&pool->_mutex);
  size_t piece_index = pool->num_pieces + 1;
  for (size_t i = 0; i < pool->num_pieces; i++) {
    if (!has_piece(pool->current_bitfield, i)) {
      set_piece(pool->current_bitfield, i);
      piece_index = i;
      break;
    }
  }
  pthread_mutex_unlock(&pool->_mutex);
  return piece_index;
}

void mark_as_undone(pieces_pool* pool, size_t index)
{
  pthread_mutex_lock(&pool->_mutex);
  unset_piece(pool->current_bitfield, index);
  pthread_mutex_unlock(&pool->_mutex);
}

bool is_done(pieces_pool* pool)
{
  pthread_mutex_lock(&pool->_mutex);
  size_t pieces_sum = 0;
  for (size_t i = 0; i < pool->num_pieces; i++) {
    if (has_piece(pool->current_bitfield, i)) {
      pieces_sum++;
    }
  }
  pthread_mutex_unlock(&pool->_mutex);
  return pieces_sum == pool->num_pieces;
}

size_t get_num_undone_pieces(pieces_pool* pool)
{
  pthread_mutex_lock(&pool->_mutex);
  size_t pieces_sum = 0;
  for (size_t i = 0; i < pool->num_pieces; i++) {
    if (has_piece(pool->current_bitfield, i)) {
      pieces_sum++;
    }
  }
  pthread_mutex_unlock(&pool->_mutex);
  return pool->num_pieces - pieces_sum;
}

void free_pieces_pool(pieces_pool* pool)
{
  pthread_mutex_lock(&pool->_mutex);
  if (pool == NULL) return;
  pthread_mutex_destroy(&pool->_mutex);
  free_bitfield(pool->current_bitfield);
  free(pool);
}
