#pragma once

typedef struct
{
  // hold the entire string to tokenize
  char* data;
  // point to the char from which the next next() call is executed
  char* current;
} tokenizer;

tokenizer* create_tokenizer(const char* data);

void destroy_tokenizer(tokenizer* t);

char* next(tokenizer* t);

