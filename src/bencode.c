#include "bencode.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* copy a substr of size len from tokenizer->current into t->token */
void copy_token(tokenizer *t, size_t len)
{
  t->token = realloc(t->token, len + 1);
  strncpy(t->token, t->current, len);
  t->token[len] = '\0';
}

int tokenize_start(tokenizer *);
int tokenize_end(tokenizer *);
int tokenize_error(tokenizer *);
int tokenize_int_start(tokenizer *);
int tokenize_int_parse(tokenizer *);
int tokenize_int_end(tokenizer *);
int tokenize_str_start(tokenizer *);
int tokenize_str_parse(tokenizer *);

int tokenize_start(tokenizer *t)
{
  if (*t->current == 'i') {
    t->status_fn = &tokenize_int_start;
  } else if (isdigit(*t->current)) {
    t->status_fn = &tokenize_str_start;
  } else if (*t->current == '\0') {
    t->status_fn = &tokenize_end;
  } else {
    t->status_fn = &tokenize_error;
  }
  return t->status_fn(t);
}

int tokenize_end(tokenizer *t) { return TOKENIZER_END; }

int tokenize_error(tokenizer *t) { return TOKENIZER_MALFORMED_STRING; }

int tokenize_int_start(tokenizer *t)
{
  if (*t->current == 'i') {
    copy_token(t, 1);
    t->current++;
    t->status_fn = &tokenize_int_parse;
    return TOKENIZER_OK;
  }
  t->status_fn = &tokenize_error;
  return t->status_fn(t);
}

int tokenize_int_parse(tokenizer *t)
{
  char *p = t->current;
  while (isdigit(*p) || *p == '-') {
    p++;
  }
  copy_token(t, p - t->current);
  t->current = p;
  t->status_fn = &tokenize_int_end;
  return TOKENIZER_OK;
}

int tokenize_int_end(tokenizer *t)
{
  if (*t->current == 'e') {
    copy_token(t, 1);
    t->current++;
    t->status_fn = &tokenize_start;
    return TOKENIZER_OK;
  }
  t->status_fn = &tokenize_error;
  return t->status_fn(t);
}

int tokenize_str_start(tokenizer *t)
{
  t->strlen = strtol(t->current, &t->current, 10);
  if (*t->current != ':') {
    t->status_fn = &tokenize_error;
    return t->status_fn(t);
  }
  t->current++;  // skip ':'
  t->token = realloc(t->token, 2);
  strcpy(t->token, "s");
  t->status_fn = &tokenize_str_parse;
  return TOKENIZER_OK;
}

int tokenize_str_parse(tokenizer *t)
{
  copy_token(t, t->strlen);
  t->current += t->strlen;
  t->strlen = 0;
  t->status_fn = &tokenize_start;
  return TOKENIZER_OK;
}

tokenizer *init_tokenizer(const char *data)
{
  tokenizer *t = malloc(sizeof(tokenizer));
  t->data = malloc(strlen(data) + 1);
  strcpy(t->data, data);
  t->current = t->data;
  t->token = NULL;
  t->status_fn = &tokenize_start;
  return t;
}

void free_tokenizer(tokenizer *t)
{
  free(t->data);
  // t->current points to the same memory location of t->data, no need
  // to free
  if (t->token != NULL) {
    free(t->token);
  }
  free(t);
}

int next(tokenizer *t) { return t->status_fn(t); }
