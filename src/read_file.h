#pragma once

#include <stdlib.h>

// Content of a file. data is NOT a null terminated string.
typedef struct
{
  size_t size;
  char *data;
} file_buf;

// Read the entire file and put its content in file_buf.
// This method will fail for files >= 4GB, but it shouldn't be our case.
file_buf *read_file(const char *);

void free_file_buf(file_buf *f);
