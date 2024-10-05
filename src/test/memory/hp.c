

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __USE_XOPEN2K 1

#include <pthread.h>
#include <semaphore.h>

pthread_barrier_t alloc_barrier;
pthread_barrier_t fill_barrier;
pthread_barrier_t done_barrier;


#define CHECK_RESULT(res)                                                                                              \
    if (res) {                                                                                                         \
        printf("thread erro happended\n");                                                                             \
        exit(1);                                                                                                       \
    }

int TRUE = 1;
int FALSE = 0;

char *shared_buff;
unsigned int shared_sz;

void *alloc_thread_body(void *arg) {
    shared_sz = 20;
    shared_buff = (char *) malloc(sizeof(char) * shared_sz);

    // wait for allocated memory
    pthread_barrier_wait(&alloc_barrier);

    return NULL;
}

void *fill_thread_body(void *arg) {
    // wait for alloceated memory
    pthread_barrier_wait(&alloc_barrier);

    int even = *((int *) arg);
    char c = 'a';
    int start = 1;

    if (even) {
        c = 'Z';
        start = 0;
    }

    for (size_t i = start; i < shared_sz; i += 2) {
        shared_buff[i] = even ? c-- : c++;
    }

    shared_buff[shared_sz - 1] = '\0';

    // setup fill barrier ON
    pthread_barrier_wait(&fill_barrier);

    return NULL;
}

void *print_body(void *arg) {
    // wait FILL barrier
    pthread_barrier_wait(&fill_barrier);

    //printf("p.ptr=%p\n", shared_buff);
    printf("Output=%s\n", shared_buff);

    pthread_barrier_wait(&done_barrier);
    return NULL;
}

void *cln(void *arg) {
    pthread_barrier_wait(&done_barrier);
    //printf("cln.free\n");
    free(shared_buff);

    pthread_barrier_destroy(&alloc_barrier);
    pthread_barrier_destroy(&fill_barrier);
    pthread_barrier_destroy(&done_barrier);

    return NULL;
}

int main() {
    shared_buff = NULL;

    pthread_barrier_init(&alloc_barrier, NULL, 3);
    pthread_barrier_init(&fill_barrier, NULL, 3);
    pthread_barrier_init(&done_barrier, NULL, 2);

    pthread_t alloc_t;
    pthread_t even_fill_t;
    pthread_t odd_fill_t;
    pthread_t print_t;
    pthread_t cln_t;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    CHECK_RESULT(res);

    res = pthread_create(&alloc_t, &attr, alloc_thread_body, NULL);
    CHECK_RESULT(res);

    res = pthread_create(&even_fill_t, &attr, fill_thread_body, &TRUE);
    CHECK_RESULT(res);

    res = pthread_create(&odd_fill_t, &attr, fill_thread_body, &FALSE);
    CHECK_RESULT(res);

    //printf("main.print\n");
    res = pthread_create(&print_t, &attr, print_body, NULL);
    CHECK_RESULT(res);

    res = pthread_create(&cln_t, &attr, cln, NULL);
    CHECK_RESULT(res);

    pthread_exit(NULL);

    return 0;
}