#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p[2], status;
    pipe(p);

    if (fork() == 0) {
        // child
        int pid = getpid();

        char buf[512];
        int n;

        if ((n = read(p[0], buf, sizeof buf)) < 0) {
            fprintf(2, "read error\n");
            exit(1);
        }
        close(p[0]);
        fprintf(2, "%d: received %s\n", pid, buf);

        char *pong[] = {"pong", 0};
        n = sizeof pong;
        if (write(p[1], *pong, n) != n) {
                fprintf(2, "write error\n");
                exit(1);
        }
        close(p[1]);

    } else {
        // parent
        int pid = getpid();

        char buf[512];
        int n;

        char *ping[] = {"ping", 0};
        n = sizeof ping;
        if (write(p[1], *ping, n) != n) {
                fprintf(2, "write error\n");
                exit(1);
        }
        close(p[1]);

        wait(&status);

        if ((n = read(p[0], buf, sizeof buf)) < 0) {
            fprintf(2, "read error\n");
            exit(1);
        }
        close(p[0]);
        fprintf(2, "%d: received %s\n", pid, buf);
    }

    exit(0);
}