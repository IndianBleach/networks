
#include <pthread.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// join
// detached threads

// pstree

int main() {
    int i = 0;

    printf("cur_pid=%i\n", getpid());

    int ret = fork();
    if (ret) {
        printf(" sub proccess spawned with pid=%i\n", ret);
    } else {
        printf("this is sub process. pid=%i\n", getpid());
    }

    printf("end=%i\n", getpid());

    while (1)
        ;


    return 0;
}