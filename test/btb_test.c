#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/bencode.h"

void test_tokenize_int(void)
{
  tokenizer *t = create_tokenizer("i450e");
  char *got1 = next(t);
  char want1[] = "i";
  if (strcmp(got1, want1) != 0) {
    fprintf(stderr, "next failed: got %s, want %s\n", got1, want1);
  }
  char *got2 = next(t);
  char want2[] = "450";
  if (strcmp(got2, want2) != 0) {
    fprintf(stderr, "next failed: got %s, want %s\n", got2, want2);
  }
  char *got3 = next(t);
  char want3[] = "e";
  if (strcmp(got3, want3) != 0) {
    fprintf(stderr, "next failed: got %s, want %s\n", got3, want3);
  }
  char *got4 = next(t);
  char *want4 = NULL;
  if (got4 != want4) {
    fprintf(stderr, "next failed: got %s, want %s\n", got4, want4);
  }
  destroy_tokenizer(t);
}

int main(void) { test_tokenize_int(); }
