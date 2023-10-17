#include "kernel/types.h"
#include "user/user.h"

#define ITERATIONS 1000

int
main(void)
{
    int fds1[2];
    int fds2[2];
    pipe(fds1);
    pipe(fds2);
    if (fork() == 0) {
        char rbuf[1];
        char wbuf[1] = { 0 };
        for (int i=0; i < ITERATIONS; i++) {
            write(fds1[1], wbuf, 1);
            read(fds2[0], rbuf, 1);
            if (wbuf[0] != rbuf[0])
              exit(1);
        }

      close(fds1[1]);
      close(fds2[0]);
    } else {
        char rbuf[1];
        int up1 = uptime();
        for (int i=0; i < ITERATIONS; i++) {
            read(fds1[0], rbuf, 1);
            write(fds2[1], rbuf, 1);
        }
        close(fds1[0]);
        close(fds2[1]);
        printf("%d\n", ITERATIONS / (uptime() - up1));
    }

    return 0;
}
