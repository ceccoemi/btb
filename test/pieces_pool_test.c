#include "pieces_pool_test.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../src/defer.h"
#include "../src/pieces_pool.h"

void test_pieces_pool()
{
  size_t num_pieces = 10;
  pieces_pool *pp = init_pieces_pool(num_pieces);
  DEFER({ free_pieces_pool(pp); });
  if (is_done(pp)) {
    fprintf(stderr, "is_done should return false\n");
  }
  if (get_num_undone_pieces(pp) != num_pieces) {
    fprintf(stderr, "wrong num \"undone\" pieces: got %lu, want %lu\n", get_num_undone_pieces(pp),
            num_pieces);
  }
  size_t idx = get_piece_index(pp);
  if (idx >= num_pieces) {
    fprintf(stderr, "wrong index: got %li, want x < %li\n", idx, num_pieces);
  }
  if (get_num_undone_pieces(pp) != num_pieces - 1) {
    fprintf(stderr, "wrong num \"undone\" pieces: got %lu, want %lu\n", get_num_undone_pieces(pp),
            num_pieces - 1);
  }
  // Extract all pieces
  for (size_t i = 0; i < num_pieces - 1; i++) {
    idx = get_piece_index(pp);
    if (idx >= num_pieces || idx == 0) {
      fprintf(stderr, "wrong index: got %li, want 0 < x < %li\n", idx, num_pieces);
    }
  }
  if (!is_done(pp)) {
    fprintf(stderr, "is_done should return true\n");
  }
  idx = get_piece_index(pp);
  if (idx < num_pieces) {
    fprintf(stderr, "get_piece_index should return >= num_pieces\n");
  }
  mark_as_undone(pp, 0);
  if (is_done(pp)) {
    fprintf(stderr, "is_done should return false\n");
  }
  idx = get_piece_index(pp);
  if (idx != 0) {
    fprintf(stderr, "wrong index: got %li, want 0\n", idx);
  }
}

struct thread_data
{
  pieces_pool *pp;
};

static void *thread_fun(void *d)
{
  struct thread_data *data = (struct thread_data *)d;
  for (size_t i = 0; i < 100; i++) {
    size_t index = get_piece_index(data->pp);
    mark_as_undone(data->pp, index);
    get_num_undone_pieces(data->pp);
  }
  return NULL;
}

void test_pieces_pool_multi_threading()
{
  // If this is increased, the test is more affordable,
  // but if it's executed with valgrind the performance are too bad.
  size_t num_threads = 100;
  pieces_pool *pp = init_pieces_pool(num_threads);
  DEFER({ free_pieces_pool(pp); });
  struct thread_data data = {pp};
  pthread_t thread_ids[num_threads];
  for (size_t i = 0; i < num_threads; i++) {
    pthread_create(&thread_ids[i], NULL, thread_fun, &data);
  }
  for (size_t i = 0; i < num_threads; i++) {
    pthread_join(thread_ids[i], NULL);
  }
  if (is_done(pp) && get_num_undone_pieces(pp) != num_threads) {
    fprintf(stderr, "pieces_pool should have %lu pieces left, got %lu\n", num_threads,
            get_num_undone_pieces(pp));
  }
}