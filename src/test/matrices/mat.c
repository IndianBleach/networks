#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct mat3 {
    int x, y, z;
    int x2, y2, z2;
    int x3, y3, z3;
} mat3;

/*
    init
    ts_summ()
    ts_diff()
    ts_mul
    ts_det
*/

void mat3_init(mat3 *self, int x, int y, int z, int x2, int y2, int z2, int x3, int y3, int z3) {
    self->x = x;
    self->x2 = x2;
    self->x3 = x3;

    self->y = y;
    self->y2 = y2;
    self->y3 = y3;

    self->z = z;
    self->z2 = z2;
    self->z3 = z3;
}

void mat3_summ(mat3 *_ma, mat3 _mx) {
    /*
            thread_
    */
}
