#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/bencode.h"

void test_tokenize_int(void)
{
  char input_str[] = "i8ei-450e";
  tokenizer *t = init_tokenizer(input_str);

  char *want = malloc(strlen(input_str) + 1);
  int err;

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 1 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "i");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 1 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 2 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "8");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 2 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 3 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "e");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 3 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 4 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "i");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 4 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 5 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "-450");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 5 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 6 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "e");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 6 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "next 7 failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

  free(want);
  free_tokenizer(t);
}

void test_tokenize_str(void)
{
  char input_str[] = "2:hi8:mystring";
  tokenizer *t = init_tokenizer(input_str);

  char *want = malloc(strlen(input_str) + 1);
  int err;

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 1 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "s");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 1 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 2 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "hi");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 2 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 3 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "s");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 3 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 4 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "mystring");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 4 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "next 5 failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

  free(want);
  free_tokenizer(t);
}

void test_tokenize_int_str(void)
{
  char input_str[] = "i34e10:holaholabb";
  tokenizer *t = init_tokenizer(input_str);

  char *want = malloc(strlen(input_str) + 1);
  int err;

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 1 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "i");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 1 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 2 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "34");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 2 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 3 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "e");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 3 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 4 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "s");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 4 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next 5 failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "holaholabb");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong 5 token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "next 6 failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

  free(want);
  free_tokenizer(t);
}

int main(void)
{
  fprintf(stdout, "Running tests...\n");
  fprintf(stdout, "\ttest_tokenize_int\n");
  test_tokenize_int();
  fprintf(stdout, "\ttest_tokenize_str\n");
  test_tokenize_str();
  fprintf(stdout, "\ttest_tokenize_int_str\n");
  test_tokenize_int_str();
  fprintf(stdout, "done");
}
