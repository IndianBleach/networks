#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vector vector;

struct vector {
    void *buff;
    size_t capacity;
    size_t size;
    size_t type_size;
};

void vector_init(vector *vec, size_t base_cap, size_t type_sz) {
    vec->buff = (void *) malloc(sizeof(void *) * base_cap);
    if (!vec->buff) {
        fprintf(stderr, "vector_init: malloc error\n");
        return;
    }
    vec->type_size = type_sz;
    vec->size = 0;
    vec->capacity = base_cap;
}

void vector_dstr(vector *vec) {}

void vector_ensure_capacity(vector *vec, size_t need) {
    printf("ensure: %i\n", need);
    vec->buff = (void *) realloc((char *) vec->buff, vec->type_size * need);
    if (!vec->buff) {
        printf("vector_ensure_capacity=error\n");
        return;
    }
    vec->capacity = need;
}

void vector_push(vector *vec, void *elem) {
    if (elem == NULL) {
        return;
    }

    // cap=count allocated links
    if (vec->size == vec->capacity) {
        vector_ensure_capacity(vec, vec->capacity * 2);
        // x2
        printf("vector_push.resize\n");
    }


    printf("new: cap=%i buff=%p\n", vec->capacity, vec->buff);

    memcpy(vec->buff + vec->size, &elem, vec->type_size);
    vec->size++;
}

void *vector_at(vector *vec, size_t index) {
    if (index > vec->size) {
        return NULL;
    }

    void *ind = vec->buff + (index);
    printf("vector_at: %p\n", ind);

    return ind;
}

// push

// new
// dst

int main() {
    printf("main.start\n");


    int t = 5;
    int t2 = 9;
    int t3 = 190;

    printf("SIZE=%i\n", sizeof(int *));
    printf("SIZE=%i\n", sizeof(int *));

    // alocating block of memory
    // representig variable begin block as void ptr
    void *buff = (void *) malloc(sizeof(int *) * 2);
    void *ofs = buff;

    printf("begin=%p end=%p\n", buff, buff + sizeof(void *) * 2);
    printf("PTR=%p\n", ofs);

    // copy VARIABLE(ADDRESS) ( void*) to BLOCK
    memcpy(buff, &t, sizeof(int));
    memcpy(buff + sizeof(void *), &t2, sizeof(int));


    ofs = realloc(buff, sizeof(int *) * 4);
    memcpy(ofs + sizeof(void *) * 2, &t3, sizeof(int));

    int r1 = *(int *) (ofs + 0);
    int r2 = *(int *) (ofs + sizeof(void *));
    int r3 = *(int *) (ofs + sizeof(void *) * 2);

    printf("t=%i\n", r1);
    printf("t=%i\n", r2);
    printf("t=%i\n", r3);

    //memcpy((char) buff + sizeof(int) * 1, &t, sizeof(int));
    //memcpy((char) buff + sizeof(int) * 2, &t2, sizeof(int));

    //int r1 = *(int *) (buff + 0);

    //void *ptr = (void *) realloc((void *) buff, sizeof(void *) * 4);

    //int r2 = *(int *) (ptr + 0);

    //printf("t=%i\n", r1);

    /*
    vector vec;
    vector_init(&vec, 2, sizeof(int));

    vector_push(&vec, &t);
    vector_push(&vec, &t2);

    int *ret1 = (int *) vector_at(&vec, 0);

    vector_push(&vec, &t2);

    //int *ret2 = (int *) vector_at(&vec, 0);

    printf("AT=%i\n", *ret1);

    printf("main.end\n");
*/

    return 0;
}