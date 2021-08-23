#pragma once

#include <stddef.h>

#define PIECE_INDEX_BYTES 4
#define BLOCK_OFFSET_BYTES 4
#define BLOCK_LENGTH_BYTES 4
#define BLOCK_SIZE 16384  // 2^14

typedef struct piece_progress
{
  size_t index;
  size_t size;
  char *buf;
  // Number of downloaded blocks
  size_t downloaded;
} piece_progress;

piece_progress *init_piece_progress(size_t index, size_t piece_size);

void free_piece_progress(piece_progress *);
