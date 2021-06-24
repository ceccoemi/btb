#pragma once

#include <stdbool.h>
#include <stdlib.h>

typedef struct bitfield
{
  size_t length;
  unsigned char *data;
} bitfield;

bitfield *init_bitfield(unsigned char *, size_t);

bool has_piece(bitfield *, size_t);

void set_piece(bitfield *, size_t);

void unset_piece(bitfield *, size_t);

void free_bitfield(bitfield *);
