#include "bencode.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tokenizer *init_tokenizer(const char *data)
{
  tokenizer *t = malloc(sizeof(tokenizer));
  t->data = malloc(strlen(data) + 1);
  strcpy(t->data, data);
  t->current = t->data;
  return t;
}

void free_tokenizer(tokenizer *t)
{
  free(t->data);
  // t->current points to the same memory location of t->data, no need to free
  if (t->token != NULL) {
    free(t->token);
  }
  free(t);
}

void copy_token(tokenizer *t, size_t len)
{
  t->token = realloc(t->token, len + 1);
  strncpy(t->token, t->current, len);
  t->token[len] = '\0';
}

int next(tokenizer *t)
{
  if (*t->current == '\0') {
    return TOKENIZER_END;
  }
  if (*t->current == 'i' || *t->current == 'e') {
    copy_token(t, 1);
    t->current++;
    return TOKENIZER_OK;
  }
  char *p = t->current;
  while (isdigit(*p) || *p == '-') {
    if (*p == '\0') {
      return TOKENIZER_MALFORMED_STRING;
    }
    p++;
  }
  copy_token(t, p - t->current);
  t->current = p;
  return TOKENIZER_OK;
}
