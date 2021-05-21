#include "pieces_queue.h"

#include <stdlib.h>

pieces_queue* init_pieces_queue(size_t q)
{
  pieces_queue* q = malloc(sizeof(pieces_queue));
  q->num_pieces = num_pieces;
  q->current_bitfield = calloc(sizeof(q->num_pieces));
  return q;
}

int get_piece_index(pieces_queue* q) { return 0; }

bool is_done(pieces_queue* q) { return false; }

void free_pieces_queue(pieces_queue* q)
{
  if (q != NULL) {
    if (q->current_bitfield != NULL) free(q->current_bitfield);
  }
}
