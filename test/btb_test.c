#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/bencode.h"

void test_tokenize_int(void)
{
  char input_str[] = "i8ei-450e";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[6] = {"i", "8", "e", "i", "-450", "e"};
  for (int i = 0; i < 6; i++) {
    int err = next(t);
    if (err != TOKENIZER_OK) {
      fprintf(stderr, "%d-th next failed: got %d error code\n", i + 1, err);
      return;
    }
    if (strcmp(t->token, want[i]) != 0) {
      fprintf(stderr, "wrong %d-th token: got %s, want %s\n", i + 1, t->token,
              want[i]);
      return;
    }
  }

  int err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "final next failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

  free_tokenizer(t);
}

void test_tokenize_str(void)
{
  char input_str[] = "2:hi8:mystring";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[4] = {"s", "hi", "s", "mystring"};
  for (int i = 0; i < 4; i++) {
    int err = next(t);
    if (err != TOKENIZER_OK) {
      fprintf(stderr, "%d-th next failed: got %d error code\n", i + 1, err);
      return;
    }
    if (strcmp(t->token, want[i]) != 0) {
      fprintf(stderr, "wrong %d-th token: got %s, want %s\n", i + 1, t->token,
              want[i]);
      return;
    }
  }

  int err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "final next failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

  free_tokenizer(t);
}

void test_tokenize_int_str(void)
{
  char input_str[] = "i34e10:holaholabb";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[5] = {"i", "34", "e", "s", "holaholabb"};
  for (int i = 0; i < 5; i++) {
    int err = next(t);
    if (err != TOKENIZER_OK) {
      fprintf(stderr, "%d-th next failed: got %d error code\n", i + 1, err);
      return;
    }
    if (strcmp(t->token, want[i]) != 0) {
      fprintf(stderr, "wrong %d-th token: got %s, want %s\n", i + 1, t->token,
              want[i]);
      return;
    }
  }

  int err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "final next failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

  free_tokenizer(t);
}

void test_tokenize_list(void)
{
  char input_str[] = "l4:spami7ee";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[7] = {"l", "s", "spam", "i", "7", "e", "e"};
  for (int i = 0; i < 7; i++) {
    int err = next(t);
    if (err != TOKENIZER_OK) {
      fprintf(stderr, "%d-th next failed: got %d error code\n", i + 1, err);
      return;
    }
    if (strcmp(t->token, want[i]) != 0) {
      fprintf(stderr, "wrong %d-th token: got %s, want %s\n", i + 1, t->token,
              want[i]);
      return;
    }
  }

  int err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "final next failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

  free_tokenizer(t);
}

void test_tokenize_nested_list(void)
{
  char input_str[] = "lli-23eee";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[7] = {"l", "l", "i", "-23", "e", "e", "e"};
  for (int i = 0; i < 7; i++) {
    int err = next(t);
    if (err != TOKENIZER_OK) {
      fprintf(stderr, "%d-th next failed: got %d error code\n", i + 1, err);
      return;
    }
    if (strcmp(t->token, want[i]) != 0) {
      fprintf(stderr, "wrong %d-th token: got %s, want %s\n", i + 1, t->token,
              want[i]);
      return;
    }
  }

  int err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "final next failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

  free_tokenizer(t);
}

void test_tokenize_dict(void)
{
  char input_str[] = "dli-23ed4:spameee";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[11] = {"d", "l",    "i", "-23", "e", "d",
                          "s", "spam", "e", "e",   "e"};
  for (int i = 0; i < 11; i++) {
    int err = next(t);
    if (err != TOKENIZER_OK) {
      fprintf(stderr, "%d-th next failed: got %d error code\n", i + 1, err);
      return;
    }
    if (strcmp(t->token, want[i]) != 0) {
      fprintf(stderr, "wrong %d-th token: got %s, want %s\n", i + 1, t->token,
              want[i]);
      return;
    }
  }

  int err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "final next failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

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
  fprintf(stdout, "\ttest_tokenize_list\n");
  test_tokenize_list();
  fprintf(stdout, "\ttest_tokenize_nested_list\n");
  test_tokenize_nested_list();
  // fprintf(stdout, "\ttest_tokenize_dict\n");
  // test_tokenize_dict();
  fprintf(stdout, "done\n");
}
