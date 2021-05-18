#pragma once

#include <stdbool.h>
#include <stdlib.h>

typedef struct bitfield
{
  size_t length;
  unsigned char *data;
} bitfield;

bitfield *init_bitfield(unsigned char *, size_t);

bool has_piece(bitfield *, int);

void free_bitfield(bitfield *);
