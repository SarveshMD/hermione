#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    int sleep_time = 30;

    if (pid == 0) {
        printf("Child exiting... pid: %d, getpid(): %d, getppid(): %d\n", pid, getpid(), getppid());
        exit(0);
    }
    else if (pid > 0) {
        printf("Parent sleeping for %d seconds without wait()... pid: %d, getpid(): %d, getppid(): %d\n", sleep_time, pid, getpid(), getppid());
        sleep(sleep_time);

        printf("Calling wait()...\n");

        int exit_status;
        int child_pid = wait(&exit_status);

        if (!WIFEXITED(exit_status))
            printf("exit_status_code is unreliable\n");

        printf("Called wait... exit_status_code: %d, child_pid: %d\n", WEXITSTATUS(exit_status), child_pid);
        printf("Parent sleeping for another %d seconds, after wait()...\n", sleep_time);
        sleep(sleep_time);
    }
}
