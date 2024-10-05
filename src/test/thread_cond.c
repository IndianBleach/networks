


#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct shared_state {
    bool Done;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
};

typedef struct shared_state shared_state;

void *workA(void *arg) {
    shared_state *ss = (shared_state *) arg;
    pthread_mutex_lock(&ss->mtx);
    printf("t.A\n");
    ss->Done = true;
    printf("signal\n");
    pthread_cond_signal(&ss->cond);
    pthread_mutex_unlock(&ss->mtx);
    return NULL;
}

void *workB(void *arg) {
    shared_state *ss = (shared_state *) arg;
    pthread_mutex_lock(&ss->mtx);

    while (!ss->Done) {
        printf("WAIT\n");
        pthread_cond_wait(&ss->cond, &ss->mtx);
    }

    printf("t.B\n");
    pthread_mutex_unlock(&ss->mtx);

    return NULL;
}

int main() {
    // init
    shared_state ss;
    ss.Done = false;
    pthread_mutex_init(&ss.mtx, NULL);
    pthread_cond_init(&ss.cond, NULL);


    pthread_t t2;
    pthread_create(&t2, NULL, workB, &ss);

    pthread_t t1;
    pthread_create(&t1, NULL, workA, &ss);


    // join
    pthread_join(t2, NULL);
    pthread_join(t1, NULL);


    // free
    pthread_mutex_destroy(&ss.mtx);
    pthread_cond_destroy(&ss.cond);

    return 0;
}