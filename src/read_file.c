#include "file_buf.h"

file_buf *read_file(const char *fname) {}

void free_file_buf(file_buf *f)
{
  if (f != NULL) {
    if (f->data != NULL) free(data);
    free(f);
  }
