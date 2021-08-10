#include "file_buf_test.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../src/defer.h"
#include "../src/file_buf.h"

#pragma GCC diagnostic ignored "-Wformat"

void test_read_file()
{
  file_buf *f = read_file("test/data/test.txt");
  DEFER({ free_file_buf(f); });
  if (f == NULL) {
    fprintf(stderr, "error in read_file\n");
  }
  char wantText[] = "This is a test file\n";
  if (f->size != strlen(wantText)) {
    fprintf(stderr, "wrong file_buf->size: got %d, want %d\n", f->size, strlen(wantText));
  }
  if (memcmp(f->data, wantText, strlen(wantText)) != 0) {
    fprintf(stderr, "wrong file content: got %.*s, want %s\n", f->size, f->data, wantText);
  }
}

void test_write_file(void)
{
  size_t size = 16;
  char data[size];  // {0, 1, 2, ..., size - 1}
  for (size_t i = 0; i < size; i++) {
    data[i] = i;
  }

  file_buf *fb = init_file_buf(data, size);
  const char *output_fname = "test/data/out.txt";
  bool ok = write_file(fb, output_fname);
  if (!ok) {
    fprintf(stderr, "write_file failed\n");
  }
  if (access(output_fname, F_OK) == -1) {
    fprintf(stderr, "the output file %s doesn't exist\n", output_fname);
  }
  DEFER({ free_file_buf(fb); });
}