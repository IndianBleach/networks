#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __USE_XOPEN2K 600

/*
В классическом примере речь идет о создании 50 молекул воды. Чтобы их получить,
нам понадобится 50 атомов кислорода и 100 атомов водорода.

В следующем коде мы сначала создаем 50 потоков для кислорода и 100 для водорода. В первых критические участки будут защищены мьютексом, а во вторых —
семафором общего вида, который позволяет находиться на критическом участке
сразу двум потокам.
*/

// spec
#include <pthread.h>
#include <semaphore.h>

pthread_barrier_t water_barrier;

pthread_mutex_t oxy_mutex;

sem_t *hydrogen_sem;

unsigned int num_created;

void *hydrogen_thread_body(void *arg) {
    // wait TWO mols of hydrogen
    printf("hydr.sem_wait\n");
    sem_wait(hydrogen_sem);
    // wait ONE mol water
    printf("hydr.bar_wait\n");
    pthread_barrier_wait(&water_barrier);

    sem_post(hydrogen_sem);
    return NULL;
}

void *oxygen_thread_body(void *arg) {
    pthread_mutex_lock(&oxy_mutex);
    printf("oxy.pthread_mutex_lock\n");

    printf("oxy.wait\n");
    pthread_barrier_wait(&water_barrier);

    // CREATE
    num_created++;
    pthread_mutex_unlock(&oxy_mutex);
    return NULL;
}

int main() {
    num_created = 0;
    pthread_mutex_init(&oxy_mutex, NULL);

    sem_t loc_sem;
    hydrogen_sem = &loc_sem;
    sem_init(hydrogen_sem, 0, 2);

    pthread_barrier_init(&water_barrier, NULL, 3);

    pthread_t tids[150];
    // threads for oxygens
    for (int i = 0; i < 1; i++) {
        if (pthread_create(tids + i, NULL, oxygen_thread_body, NULL)) {
            printf("Some error\n");
            exit(1);
        }
    }

    // threads for hydrogens
    for (int i = 1; i < 3; i++) {
        if (pthread_create(tids + i, NULL, hydrogen_thread_body, NULL)) {
            printf("Some error\n");
            exit(2);
        }
    }

    for (int i = 2; i >= 0; i--) {
        if (pthread_join(tids[i], NULL)) {
            printf("Some error (JOIN)\n");
            exit(3);
        }
    }

    printf("created mols: %i\n", num_created);

    sem_destroy(hydrogen_sem);

    return 0;
}