#include "file_buf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defer.h"

file_buf *read_file(const char *fname)
{
  FILE *f = fopen(fname, "rb");
  if (f == NULL) {
    fprintf(stderr, "Error in opening the file %s\n", fname);
    return NULL;
  }
  DEFER({ fclose(f); });
  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  file_buf *buf = malloc(sizeof(file_buf));
  buf->data = malloc(length);
  buf->size = length;
  long r = fread(buf->data, 1, length, f);
  if (r != length) {
    fprintf(stderr, "Error: read %ld of %ld bytes from file %s\n", r, length, fname);
    free_file_buf(buf);
    return NULL;
  }
  return buf;
}

void free_file_buf(file_buf *f)
{
  if (f != NULL) {
    if (f->data != NULL) free(f->data);
    free(f);
  }
}
