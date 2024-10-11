

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

#include <mqueue.h>

int main3() {
    mqd_t mq;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 32;
    attr.mq_curmsgs = 0;

    int child = fork();
    if (child == -1) {
        printf("fork error\n");
        exit(1);
    }

    if (child == 0) {
        // children
        // CONNECT TO EXISTS QUEUE
        sleep(1);
        mq = mq_open("/mq0", O_RDWR);
        char str[] = "Hello VI!\n";
        printf("wait\n");
        sleep(2);
        mq_send(mq, str, strlen(str) + 1, 0);


        char buff[32];
        int nreads = mq_receive(mq, buff, 32, NULL);
        printf("receive2=%s\n", buff);

        mq_close(mq);
    } else {
        // CREATE QUEUE
        mq = mq_open("/mq0", O_RDWR | O_CREAT, 0644, &attr);
        char buff[32];
        int nreads = mq_receive(mq, buff, 32, NULL);
        printf("receive=%s\n", buff);


        char v2[] = "msg2";
        mq_send(mq, v2, strlen(v2) + 1, 0);

        mq_close(mq);
    }

    return 0;
}