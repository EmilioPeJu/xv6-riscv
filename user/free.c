#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    printf("%d\n", freemem());
    exit(0);
}
