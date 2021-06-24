#pragma once

#include "bitfield.h"

typedef struct pieces_queue
{
  bitfield* current_bitfield;
  size_t num_pieces;
} pieces_queue;

pieces_queue* init_pieces_queue(size_t);

// Get and mark as done the first "undone" piece index.
size_t get_piece_index(pieces_queue*);

// Set the input index as "undone",
// So it can be extracted using get_piece_index.
void mark_as_undone(pieces_queue*, size_t);

bool is_done(pieces_queue*);

void free_pieces_queue(pieces_queue*);
