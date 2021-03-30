#include "bencode.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tokenizer *create_tokenizer(const char *data)
{
  tokenizer *t = malloc(sizeof(tokenizer));
  t->data = malloc(strlen(data) + 1);
  strcpy(t->data, data);
  t->current = t->data;
  return t;
}

void destroy_tokenizer(tokenizer *t)
{
  free(t->data);
  // t->current point to the same memory location of t->data, no need to free it
  free(t);
}

char *next(tokenizer *t)
{
  if (*t->current == '\0') {
    return NULL;
  }
  if (*t->current == 'i' || *t->current == 'e') {
    char *rst = malloc(2);
    strncpy(rst, t->current, 1);
    t->current++;
    return rst;
  }
  char *p = t->current;
  while (isdigit(*p)) {
    if (*p == '\0') {
      return NULL;  // ERROR: malformed string
    }
    p++;
  }
  char *rst = malloc(p - t->current + 1);
  strncpy(rst, t->current, p - t->current);
  t->current = p;
  return rst;
}
