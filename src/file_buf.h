#pragma once

#include <stdbool.h>
#include <stdlib.h>

// Content of a file. data is NOT a null terminated string.
typedef struct
{
  size_t size;
  char *data;
} file_buf;

file_buf *init_file_buf(char *data, size_t size);

// Read the entire file and put its content in file_buf.
// This method will fail for files >= 4GB.
file_buf *read_file(const char *);

// Write the file buffer into the file "output_fname".
// It overrides its content if the file already exists.
// It returns true if it succeeds.
bool write_file(file_buf *fb, const char *output_fname);

void free_file_buf(file_buf *);
