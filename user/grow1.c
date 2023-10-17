#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    printf("page table before\n");
    showpt(0);
    char *addr = sbrk(1);
    printf("memory %p:\n", addr);
    for (int i=0; i < 64; i++)
        printf("%d ", (int) *(addr + i));

    printf("\n");
    printf("page table after\n");
    showpt(0);
    return 0;
}
