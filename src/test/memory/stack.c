

#define __USE_XOPEN2K 1

#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *body1(void *arg) {
    int v = 1;
    int *stack_ptr = &v;
    pthread_t tid = pthread_self();
    printf("thread=%i stack=%p\n", tid, stack_ptr);

    return NULL;
}

void *body2(void *arg) {
    int v = 1;
    int *stack_ptr = &v;
    pthread_t tid = pthread_self();
    printf("thread2=%i stack=%p\n", tid, stack_ptr);

    return NULL;
}

#define PTHREAD_STACK_MIN 8192

int main() {
    // init attr
    // init buffer
    // set_stack_attr
    // create thread 1
    // create thread 2 + attr


    size_t buff_len = (PTHREAD_STACK_MIN * 1024) / 4;
    char *buff = (char *) malloc(sizeof(char) * buff_len);

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t tid1;
    pthread_t tid2;

    //pthread_attr_setstack()

    if (pthread_attr_setstacksize(&attr, buff_len)) {
        printf("pthread_attr_setstacksize\n");
    }

    if (pthread_attr_setstackaddr(&attr, buff)) {
        printf("pthread_attr_setstackaddr\n");
    }


    pthread_create(&tid1, NULL, body1, NULL);
    pthread_create(&tid2, &attr, body2, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    free(buff);

    exit(0);

    return 0;
}