#include "tokenizer.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tokenize_start(tokenizer *);
int tokenize_end(tokenizer *);
int tokenize_error(tokenizer *);
int tokenize_int_start(tokenizer *);
int tokenize_int_parse(tokenizer *);
int tokenize_int_end(tokenizer *);
int tokenize_str_start(tokenizer *);
int tokenize_str_parse(tokenizer *);
int tokenize_list_dict_start(tokenizer *);
int tokenize_list_dict_end(tokenizer *);

int tokenize_start(tokenizer *t)
{
  if (*t->current == 'i') {
    t->_status_fn = &tokenize_int_start;
  } else if (isdigit(*t->current)) {
    t->_status_fn = &tokenize_str_start;
  } else if (*t->current == 'l' || *t->current == 'd') {
    t->_status_fn = &tokenize_list_dict_start;
  } else if (*t->current == 'e') {
    t->_status_fn = &tokenize_list_dict_end;
  } else if (isspace(*t->current)) {
    // skip whitespaces
    t->current++;
  } else if (t->current - t->data == t->data_size) {
    if (t->list_dict_stack > 0) {
      // There are some opened lists/dicts
      t->_status_fn = &tokenize_error;
    } else {
      // Correct termination
      t->_status_fn = &tokenize_end;
    }
  } else {
    t->_status_fn = &tokenize_error;
  }
  return t->_status_fn(t);
}

int tokenize_end(tokenizer *t)
{
  t->token = NULL;
  t->token_size = 0;
  return TOKENIZER_END;
}

int tokenize_error(tokenizer *t)
{
  t->token = NULL;
  t->token_size = 0;
  return TOKENIZER_MALFORMED_STRING;
}

int tokenize_int_start(tokenizer *t)
{
  if (*t->current == 'i') {
    t->token = realloc(t->token, 1);
    memcpy(t->token, t->current, 1);
    t->token_size = 1;
    t->current++;
    t->_status_fn = &tokenize_int_parse;
    return TOKENIZER_OK;
  }
  t->_status_fn = &tokenize_error;
  return t->_status_fn(t);
}

int tokenize_int_parse(tokenizer *t)
{
  char *p = t->current;
  if (isdigit(*p) || *p == '-') {
    p++;
  }
  while (isdigit(*p)) {
    p++;
  }
  t->token = realloc(t->token, p - t->current);
  memcpy(t->token, t->current, p - t->current);
  t->token_size = p - t->current;
  t->current = p;
  t->_status_fn = &tokenize_int_end;
  return TOKENIZER_OK;
}

int tokenize_int_end(tokenizer *t)
{
  if (*t->current == 'e') {
    t->token = realloc(t->token, 1);
    memcpy(t->token, t->current, 1);
    t->token_size = 1;
    t->current++;
    t->_status_fn = &tokenize_start;
    return TOKENIZER_OK;
  }
  t->_status_fn = &tokenize_error;
  return t->_status_fn(t);
}

int tokenize_str_start(tokenizer *t)
{
  t->_strlen = strtol(t->current, &t->current, 10);
  if (*t->current != ':') {
    t->_status_fn = &tokenize_error;
    return t->_status_fn(t);
  }
  t->current++;  // skip ':'
  t->token = realloc(t->token, 1);
  memcpy(t->token, "s", 1);
  t->token_size = 1;
  t->_status_fn = &tokenize_str_parse;
  return TOKENIZER_OK;
}

int tokenize_str_parse(tokenizer *t)
{
  t->token = realloc(t->token, t->_strlen);
  memcpy(t->token, t->current, t->_strlen);
  t->token_size = t->_strlen;
  t->current += t->_strlen;
  t->_status_fn = &tokenize_start;
  return TOKENIZER_OK;
}

int tokenize_list_dict_start(tokenizer *t)
{
  if (*t->current == 'l' || *t->current == 'd') {
    t->token = realloc(t->token, 1);
    memcpy(t->token, t->current, 1);
    t->token_size = 1;
    t->current++;
    t->list_dict_stack++;
    t->_status_fn = &tokenize_start;
    return TOKENIZER_OK;
  }
  t->_status_fn = &tokenize_error;
  return t->_status_fn(t);
}

int tokenize_list_dict_end(tokenizer *t)
{
  if (t->list_dict_stack < 0) {
    // There wasn't opened lists/dicts
    t->_status_fn = &tokenize_error;
  }
  if (*t->current == 'e') {
    t->token = realloc(t->token, 1);
    memcpy(t->token, t->current, 1);
    t->token_size = 1;
    t->current++;
    t->list_dict_stack--;
    t->_status_fn = &tokenize_start;
    return TOKENIZER_OK;
  }
  t->_status_fn = &tokenize_error;
  return t->_status_fn(t);
}

tokenizer *init_tokenizer(const char *data, long unsigned data_size)
{
  tokenizer *t = malloc(sizeof(tokenizer));
  t->data_size = data_size;
  t->data = malloc(data_size);
  memcpy(t->data, data, data_size);
  t->current = t->data;
  t->token_size = 0;
  t->token = NULL;
  t->_status_fn = &tokenize_start;
  t->_strlen = 0;
  t->list_dict_stack = 0;
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

int next(tokenizer *t) { return t->_status_fn(t); }
