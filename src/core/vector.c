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

void vector_dstr(vector *vec) {
    free(vec->buff);
    //free(vec);
}

void vector_ensure_capacity(vector *vec, size_t new_cap) {
    printf("ensure: %i\n", new_cap);
    vec->buff = (void *) realloc(vec->buff, vec->type_size * new_cap);
    if (!vec->buff) {
        printf("vector_ensure_capacity=error\n");
        return;
    }
    vec->capacity = new_cap;
}

// fix
void vector_push_back(vector *vec, void *elem) {
    if (elem == NULL) {
        return;
    }

    // cap=count allocated links
    if (vec->size == vec->capacity) {
        vector_ensure_capacity(vec, vec->capacity * 2);
        // x2
        printf("vector_push.resize\n");
    }

    memcpy(vec->buff + vec->size * sizeof(void *), elem, vec->type_size);
    vec->size++;
}

void *vector_at(vector *vec, size_t index) {
    if (index > vec->size) {
        return NULL;
    }

    return vec->buff + (sizeof(void *) * index);
}

void vector_resize(vector *vec, size_t new_size) { vec->size = new_size; }

void vector_shrink_to_fit(vector *vec) {
    // delete elements by capacity
    size_t new_size = sizeof(void *) * vec->size;
    void *newb = realloc(vec->buff, new_size);
    vec->buff = newb;
}

void vector_clear(vector *vec) {
    vector_resize(vec, 0);
    vector_shrink_to_fit(vec);
}

// fix
void vector_reserve(vector *vec, size_t count) {
    size_t free_now = (vec->capacity - vec->size);

    if (free_now < count) {
        vector_ensure_capacity(vec, count);
    }
}

int main() {
    printf("main.start\n");


    int t = 5;
    int t2 = 9;
    int t3 = 190;

    printf("SIZE=%i\n", sizeof(int *));
    printf("SIZE=%i\n", sizeof(int *));

    vector vec;
    vector_init(&vec, 3, sizeof(int));
    vector_push_back(&vec, &t);
    vector_push_back(&vec, &t2);
    vector_push_back(&vec, &t3);

    int r1 = *(int *) vector_at(&vec, 0);
    int r2 = *(int *) vector_at(&vec, 1);
    int r3 = *(int *) vector_at(&vec, 2);

    printf("t=%i\n", r1);
    printf("t=%i\n", r2);
    printf("t=%i\n", r3);

    vector_dstr(&vec);


    return 0;
}