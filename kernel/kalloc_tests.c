#include "defs.h"
#include "kalloc.h"

#include "unity.h"

extern char end[];

static void
test_kinit(void)
{
  kinit();
  printf("Start of free memory: %p\n", kmem.start);
  TEST_ASSERT_GREATER_THAN(0, kmem.n_free_pages);
  TEST_ASSERT_EQUAL(0, (uint64) kmem.start % PGSIZE);
}

void
run_kalloc_tests(void)
{
  RUN_TEST(test_kinit);
}
