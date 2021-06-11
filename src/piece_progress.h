#pragma once

#include <stddef.h>

typedef struct piece_progress
{
  size_t index;
  char *buf;
  size_t downloaded;
  size_t requested;
} piece_progress;

piece_progress *init_piece_progress(size_t index, size_t piece_size);

void free_piece_progress(piece_progress *);
