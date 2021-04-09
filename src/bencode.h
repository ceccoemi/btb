#pragma once

#include <stdio.h>

#define TOKENIZER_OK 0
#define TOKENIZER_END 1
#define TOKENIZER_MALFORMED_STRING 2

typedef struct tokenizer
{
  // hold the entire string to tokenize
  char* data;
  // point to the char from which the next next() call is executed
  char* current;
  // token found when next() is called
  char* token;
  // pointer to a function that represents a tokenizer status
  int (*_status_fn)(struct tokenizer*);
  // length of the next string to parse with next()
  long int _strlen;
  // number of lists and/or dict parsed and not closed
  int _list_dict_stack;
} tokenizer;

tokenizer* init_tokenizer(const char* data);

void free_tokenizer(tokenizer* t);

int next(tokenizer* t);

