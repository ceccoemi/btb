#pragma once

#include <stdbool.h>
#include <stddef.h>

// Convert the input integer n to a big endian representation
// of buf_size bytes in the buffer buf.
bool int_to_big_endian(int n, char* buf, size_t buf_size);
