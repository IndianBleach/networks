#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

sem_t *sem;

void *thread_body(void *arg) {
    int *ptr = (int *) arg;
    sem_wait(sem);
    (*ptr)++;
    printf("%d\n", *ptr);
    sem_post(sem);
    return NULL;
}

void *thread_body2(void *arg) {
    int *ptr = (int *) arg;
    sem_wait(sem);
    (*ptr) += 2;
    printf("%d\n", *ptr);
    sem_post(sem);
    return NULL;
}

int main() {
#ifdef __APPLE__
    sem = sem_open("sem0", O_CREAT | O_EXCL, 0644, 1);
#else
    sem_t local_sem;
    sem = &local_sem;
    sem_init(sem, 0, 1);

#endif


    int i = 0;

    pthread_t tid;
    int res1 = pthread_create(&tid, NULL, thread_body, &i);

    pthread_t tid2;
    int res2 = pthread_create(&tid2, NULL, thread_body2, &i);

    pthread_join(tid, NULL);
    pthread_join(tid2, NULL);

    printf("threads.Main\n");

    pthread_t cur = pthread_self();
    printf("cur==%i\n", cur);

    sem_destroy(sem);

    return 0;
}