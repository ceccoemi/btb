#include <stdio.h>

#include "btb.h"
#include "version.h"

int main(void)
{
  printf("The sum of 3 and 5 is %d\n", sum(3, 5));
  printf("Hello from BTB version %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
}
