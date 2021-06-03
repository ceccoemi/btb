#pragma once

#include <stdio.h>

#define TOKENIZER_OK 0
#define TOKENIZER_END 1
#define TOKENIZER_MALFORMED_STRING 2

// A tokenizer to tokenize a Bencode encoding.
//
// Create the tokenizer with init_tokenizer passing the data and its size.
// The input data MUST NOT be a null-terminated string.
// Call the function next() with the tokenizer as argument.
// If next succeds, the token will be in the tokenizer->token field.
// Note that token isn't a null-terminated string, the size is specified in token_size.
typedef struct tokenizer
{
  long long data_size;
  // hold the entire data to tokenize
  char* data;
  // point to the char from which the next next() call is executed
  char* current;
  size_t token_size;
  // token found when next() is called
  char* token;
  // pointer to a function that represents a tokenizer status
  int (*_status_fn)(struct tokenizer*);
  // length of the next string to parse with next()
  long _strlen;
  // number of lists and/or dict parsed and not closed
  long list_dict_stack;
} tokenizer;

tokenizer* init_tokenizer(const char* data, long unsigned data_size);

void free_tokenizer(tokenizer* t);

int next(tokenizer* t);

