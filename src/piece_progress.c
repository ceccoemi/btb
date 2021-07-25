#include "piece_progress.h"

#include <stdlib.h>

piece_progress* init_piece_progress(size_t piece_index, size_t piece_size)
{
  piece_progress* p = malloc(sizeof(piece_progress));
  p->index = piece_index;
  p->size = piece_size;
  p->buf = malloc(piece_size);
  p->downloaded = 0;
  return p;
}

void free_piece_progress(piece_progress* p)
{
  if (p == NULL) return;
  free(p->buf);
  free(p);
}
