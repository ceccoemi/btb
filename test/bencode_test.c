#include "../src/bencode.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bencode_test.h"

void test_tokenization(char *input, long unsigned input_len, const char *want[],
                       int num_tokens)
{
  tokenizer *t = init_tokenizer(input, input_len);
  for (int i = 0; i < num_tokens; i++) {
    int err = next(t);
    if (err != TOKENIZER_OK) {
      fprintf(stderr, "%d-th next failed: got %d error code\n", i + 1, err);
      free_tokenizer(t);
      return;
    }
    if (memcmp(t->token, want[i], t->token_size) != 0) {
      fprintf(stderr, "wrong %d-th token: got %s, want %s\n", i + 1, t->token,
              want[i]);
      free_tokenizer(t);
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

void test_tokenize_int(void)
{
  char input_str[] = "i8ei-450e";
  const char *want[6] = {"i", "8", "e", "i", "-450", "e"};
  test_tokenization(input_str, strlen(input_str), want, 6);
}

void test_tokenize_str(void)
{
  char input_str[] = "2:hi8:mystring";
  const char *want[4] = {"s", "hi", "s", "mystring"};
  test_tokenization(input_str, strlen(input_str), want, 4);
}

void test_tokenize_int_str(void)
{
  char input_str[] = "i34e10:holaholabb";
  const char *want[5] = {"i", "34", "e", "s", "holaholabb"};
  test_tokenization(input_str, strlen(input_str), want, 5);
}

void test_tokenize_list(void)
{
  char input_str[] = "l4:spami7ee";
  const char *want[7] = {"l", "s", "spam", "i", "7", "e", "e"};
  test_tokenization(input_str, strlen(input_str), want, 7);
}

void test_tokenize_nested_list(void)
{
  char input_str[] = "lli-23eee";
  const char *want[7] = {"l", "l", "i", "-23", "e", "e", "e"};
  test_tokenization(input_str, strlen(input_str), want, 7);
}

void test_tokenize_dict(void)
{
  char input_str[] = "dli-23ed4:spaml2:hieeee";
  const char *want[15] = {"d", "l", "i",  "-23", "e", "d", "s", "spam",
                          "l", "s", "hi", "e",   "e", "e", "e"};
  test_tokenization(input_str, strlen(input_str), want, 15);
}

void test_tokenize_str_with_null_char()
{
  char input_str[] = "9:hola\0holai23e";
  const char *want[5] = {"s", "hola\0hola", "i", "23", "e"};
  test_tokenization(input_str, 15, want, 5);
}
