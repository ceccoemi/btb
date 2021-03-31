#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/bencode.h"

void test_tokenize_int(void)
{
  char input_str[] = "i8ei-450e";
  tokenizer *t = init_tokenizer("i8ei-450e");

  char *want = malloc(strlen(input_str) + 1);
  int err;

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "i");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "8");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "e");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "i");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "-450");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_OK) {
    fprintf(stderr, "next failed: got %d error code\n", err);
    return;
  }
  strcpy(want, "e");
  if (strcmp(t->token, want) != 0) {
    fprintf(stderr, "wrong token: got %s, want %s\n", t->token, want);
    return;
  }

  err = next(t);
  if (err != TOKENIZER_END) {
    fprintf(stderr, "next failed: got %d error code, want %d\n", err,
            TOKENIZER_END);
    return;
  }

  free(want);
  free_tokenizer(t);
}

// TODO: negative number

// void test_tokenize_str(void)
//{
//  tokenizer *t = init_tokenizer("8:mystring");
//  char *got1 = next(t);
//  char want1[] = "s";
//  if (strcmp(got1, want1) != 0) {
//    fprintf(stderr, "next failed: got %s, want %s\n", got1, want1);
//  }
//  char *got2 = next(t);
//  char want2[] = "mystring";
//  if (strcmp(got2, want2) != 0) {
//    fprintf(stderr, "next failed: got %s, want %s\n", got2, want2);
//  }
//  char *got3 = next(t);
//  char *want3 = NULL;
//  if (got3 != want3) {
//    fprintf(stderr, "next failed: got %s, want %s\n", got3, want3);
//  }
//  free_tokenizer(t);
//}

int main(void)
{
  test_tokenize_int();
  // test_tokenize_str();
}
