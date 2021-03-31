#pragma once

#define TOKENIZER_OK 0
#define TOKENIZER_END 1
#define TOKENIZER_MALFORMED_STRING 2

typedef struct
{
  // hold the entire string to tokenize
  char* data;
  // point to the char from which the next next() call is executed
  char* current;
  // token found when next() is called
  char* token;
} tokenizer;

tokenizer* init_tokenizer(const char* data);

void free_tokenizer(tokenizer* t);

int next(tokenizer* t);

