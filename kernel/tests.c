#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "tests.h"

#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

FILE *stdout = (FILE *) 0;

int
putc(int c, FILE *s)
{
  return consputc(c);
}

void
run_tests(void)
{
  printf("Running tests ...\n");
  UNITY_BEGIN();
  run_kalloc_tests();
  UNITY_END();
}
