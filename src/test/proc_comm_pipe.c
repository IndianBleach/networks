

#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <fcntl.h>
#include <features.h>
#include <pthread.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

// spec
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

#define SHM_MUTEX_NAME "/mutex1"
#define SHM_NAME "/shm0"

int main4() {
    int fds[2];
    pipe(fds);

    int child_pid = fork();
    if (child_pid == -1) {
        printf("fork error\n");
        exit(1);
    }

    if (child_pid == 0) {
        // children
        //close(fds[0]);
        char str[] = "Hello vi\n";
        printf("Send to fds[1] (write) s=%s\n", str);
        sleep(2);
        write(fds[1], str, strlen(str) + 1);
        sleep(2);
        char buff[32];
        int nreads = read(fds[0], buff, 32);
        printf("readed2=%s\n", buff);
    } else {
        // paret
        //close(fds[1]);
        char buff[32];
        int nreads = read(fds[0], buff, 32);
        printf("readed=%s\n", buff);
        sleep(2);
        char s2[] = "second mail\n";
        write(fds[1], s2, strlen(s2) + 1);
        sleep(2);
    }

    return 0;
}