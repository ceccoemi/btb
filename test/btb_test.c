#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/bencode.h"

void test_tokenization(tokenizer *t, const char *want[], int num_tokens)
{
  for (int i = 0; i < num_tokens; i++) {
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
}

void test_tokenize_int(void)
{
  char input_str[] = "i8ei-450e";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[6] = {"i", "8", "e", "i", "-450", "e"};
  test_tokenization(t, want, 6);
  free_tokenizer(t);
}

void test_tokenize_str(void)
{
  char input_str[] = "2:hi8:mystring";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[4] = {"s", "hi", "s", "mystring"};
  test_tokenization(t, want, 4);
  free_tokenizer(t);
}

void test_tokenize_int_str(void)
{
  char input_str[] = "i34e10:holaholabb";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[5] = {"i", "34", "e", "s", "holaholabb"};
  test_tokenization(t, want, 5);
  free_tokenizer(t);
}

void test_tokenize_list(void)
{
  char input_str[] = "l4:spami7ee";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[7] = {"l", "s", "spam", "i", "7", "e", "e"};
  test_tokenization(t, want, 7);
  free_tokenizer(t);
}

void test_tokenize_nested_list(void)
{
  char input_str[] = "lli-23eee";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[7] = {"l", "l", "i", "-23", "e", "e", "e"};
  test_tokenization(t, want, 7);
  free_tokenizer(t);
}

void test_tokenize_dict(void)
{
  char input_str[] = "dli-23ed4:spaml2:hieeee";
  tokenizer *t = init_tokenizer(input_str);

  const char *want[15] = {"d", "l", "i",  "-23", "e", "d", "s", "spam",
                          "l", "s", "hi", "e",   "e", "e", "e"};
  test_tokenization(t, want, 15);
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
  fprintf(stdout, "\ttest_tokenize_dict\n");
  test_tokenize_dict();
  fprintf(stdout, "done\n");
}
