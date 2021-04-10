#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/bencode.h"
#include "bencode_test.h"

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