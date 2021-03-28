#include "../src/btb.h"

#include <assert.h>
#include <stdio.h>

void test_sum(void)
{
  int result = sum(6, 4);
  assert(result == 10);
}

int main(void)
{
  printf("========== RUNNING TEST ==========\n");
  printf("test_sum ... ");
  test_sum();
  printf("ok.\n");
  printf("============== OK ================\n");
}
