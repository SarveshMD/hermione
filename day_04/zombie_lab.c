#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child exiting... pid: %d, getpid(): %d, getppid(): %d\n", pid, getpid(), getppid());
        exit(0);
    }
    else if (pid > 0) {
        printf("Parent sleeping for 30 seconds without wait()... pid: %d, getpid(): %d, getppid(): %d\n", pid, getpid(), getppid());
        sleep(30);
    }
}
