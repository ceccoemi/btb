#include "bitfield.h"

#include <string.h>

bitfield *init_bitfield(unsigned char *data, size_t length)
{
  bitfield *b = malloc(sizeof(bitfield));
  b->length = length;
  b->data = malloc(length);
  memcpy(b->data, data, length);
  return b;
}

bool has_piece(bitfield *b, size_t index)
{
  int byte_index = index / 8;
  int offset = index % 8;
  return (b->data[byte_index] >> (7 - offset) & 1) != 0;
}

void set_piece(bitfield *b, size_t index)
{
  int byte_index = index / 8;
  int offset = index % 8;
  b->data[byte_index] |= 1 << (7 - offset);
}

void free_bitfield(bitfield *b)
{
  free(b->data);
  free(b);
}
