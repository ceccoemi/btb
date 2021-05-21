#pragma once

#include "bitfield.h"

typedef struct pieces_queue
{
  bitfield* current_bitfield;
  size_t num_pieces;
} pieces_queue;

pieces_queue* init_pieces_queue(size_t);

int get_piece_index(pieces_queue*);

bool is_done(pieces_queue*);

void free_pieces_queue(pieces_queue*);
