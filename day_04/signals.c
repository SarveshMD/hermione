#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("Signal Received: %d. Not gonna do anything about it...\n", signum);
}

// Better way to do this is to use sigaction, but for our little test, this is okay...

int main() {
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGCONT, handler);
    signal(SIGTSTP, handler);

    signal(SIGKILL, handler); // DOESN'T WORK
    signal(SIGSTOP, handler); // DOESN't WORK EITHER

    printf("PID: %d\n", getpid());

    while (true) {
        printf("Sleeping...\n");
        sleep(5);
    }
}
