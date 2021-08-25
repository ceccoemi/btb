#include "file_buf.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defer.h"

file_buf *read_file(const char *fname)
{
  FILE *f = fopen(fname, "rb");
  if (f == NULL) {
    fprintf(stderr, "Error in opening the file %s: %s\n", fname, strerror(errno));
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

file_buf *init_file_buf(char *data, size_t size)
{
  file_buf *fb = malloc(sizeof(file_buf));
  fb->size = size;
  fb->data = malloc(fb->size);
  memcpy(fb->data, data, fb->size);
  return fb;
}

bool write_file(file_buf *fb, const char *output_fname)
{
  FILE *f = fopen(output_fname, "w");
  if (f == NULL) {
    fprintf(stderr, "failed to open the file %s: %s\n", output_fname, strerror(errno));
    return false;
  }
  DEFER({ fclose(f); });
  int bytes_written = fwrite(fb->data, 1, fb->size, f);
  if (bytes_written != fb->size) {
    fprintf(stderr, "failed to write to file: written %d bytes, expected %lu\n", bytes_written,
            fb->size);
    return false;
  }
  return true;
}

void free_file_buf(file_buf *f)
{
  if (f != NULL) {
    if (f->data != NULL) free(f->data);
    free(f);
  }
}
