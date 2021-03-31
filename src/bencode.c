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
  t->token = malloc(strlen(data) + 1);
  return t;
}

void free_tokenizer(tokenizer *t)
{
  free(t->data);
  // t->current points to the same memory location of t->data, no need free
  free(t->token);
  free(t);
}

int next(tokenizer *t)
{
  if (*t->current == '\0') {
    return TOKENIZER_END;
  }
  if (*t->current == 'i' || *t->current == 'e') {
    strncpy(t->token, t->current, 1);
    // strncpy isn't safe. Use strlcpy instead?
    t->token[1] = '\0';
    assert(t->token[1] == '\0');
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
  strncpy(t->token, t->current, p - t->current);
  // strncpy isn't safe. Use strlcpy instead?
  t->token[p - t->current] = '\0';
  t->current = p;
  return TOKENIZER_OK;
}
