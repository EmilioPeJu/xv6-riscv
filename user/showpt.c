#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  if (argc > 1)
    showpt(1);
  else
    showpt(0);
  return 0;
}
