
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*

    vector
    -iterator
        void vector_init(vector *vec, size_t base_cap, size_t type_sz);

        void vector_dstr(vector *vec);

        void vector_ensure_capacity(vector *vec, size_t new_cap);

        void vector_push_back(vector *vec, void *elem);

        void *vector_at(vector *vec, size_t index);

        void vector_resize(vector *vec, size_t new_size);

        void vector_shrink_to_fit(vector *vec);

        void vector_clear(vector *vec);

        void vector_reserve(vector *vec, size_t count);

    array
    -iterator()
    -
*/


void log(FILE *fd, const char *__msg, const char *__caller, const char *__file, const int __line) {
    fprintf(fd, "[%s:%i <%s>]: %s\n", __file, __line, __caller, __msg);
}

#define __log_err(msg) log(stderr, msg, __func__, __FILE__, __LINE__);
#define __log_info(msg) log(stdout, msg, __func__, __FILE__, __LINE__);

// ::::::: Iterators
#pragma region Iterators

typedef struct basic_iterator {
    void *begin;
    unsigned int diff;
    size_t len;
    size_t cur;
} basic_iterator;

#define __iter_begin(ITER, TYPE) (TYPE *) iter_begin(ITER);
#define __iter_end(ITER, TYPE) (TYPE *) iter_end(ITER);
#define __iter_cur(ITER, TYPE) (TYPE *) iter_cur(ITER);
#define __iter_set(ITER, POS) iter_set(ITER, POS);
#define __iter_next(ITER, TYPE) (TYPE *) iter_next(ITER);
#define __iter_back(ITER, TYPE) (TYPE *) iter_back(ITER);

void iter_init(basic_iterator *it, void *__begin, unsigned int __diff, size_t __len) {
    it->begin = __begin;
    it->diff = __diff;
    it->len = __len;
    it->cur = 0;
}

void iter_set(basic_iterator *it, size_t __pos) {
    if (__pos >= it->len) {
        __log_err("pos>=it.len");
    }
    it->cur = __pos;
}

void *iter_begin(basic_iterator *it) { return it->begin; }

void *iter_end(basic_iterator *it) { return (void *) it->begin + ((it->len - 1) * it->diff); }

void *iter_cur(basic_iterator *it) { return (void *) it->begin + (it->cur * it->diff); }

void *iter_next(basic_iterator *it) {
    if (it->cur >= it->len) {
        __log_err("cur>it.len");
    } else {
        it->cur++;
        return iter_cur(it);
    }
}

void *iter_back(basic_iterator *it) {
    if (it->cur >= it->len) {
        __log_err("cur>it.len");
    } else {
        it->cur--;
        return iter_cur(it);
    }
}

void *iter_realloc(basic_iterator *it, size_t _newcapacity) {
    it->begin = (void *) realloc(it->begin, (it->diff * _newcapacity));
    it->len = _newcapacity;
}

#pragma endregion

// ::::::: Containers
#pragma region Array

typedef struct array {
    const basic_iterator iterator;
} array;

#define __array_at(ARR, TYPE, INDEX) (TYPE *) array_at(ARR, INDEX);

void array_init(array *arr, size_t cap, unsigned int elem_size) {
    void *begin = malloc(elem_size * cap);
    iter_init(&arr->iterator, begin, elem_size, cap);
}

size_t array_size(array *arr) { return arr->iterator.len; }

void *array_at(array *arr, size_t __index) {
    printf("len=%i\n", arr->iterator.len);
    if (__index >= arr->iterator.len) {
        __log_err("index>arr.len");
    } else {
        iter_set(&arr->iterator, __index);
        return iter_cur(&arr->iterator);
    }
}

void array_set(array *arr, size_t __index, void *__value) {
    if (__index >= arr->iterator.len) {
        __log_err("index>arr.len");
    } else {
        iter_set(&arr->iterator, __index);
        printf("diff=%i\n", arr->iterator.diff);
        memcpy(iter_cur(&arr->iterator), __value, arr->iterator.diff);
    }
}

void array_dstr(array *arr) { free(arr->iterator.begin); }

#pragma endregion

#pragma region Vector

typedef struct vector {
    size_t size;
    const basic_iterator iterator;
    // size + dynamic memory blocks
} vector;

#define __vector_at(VEC, TYPE, INDEX) (TYPE *) vector_at(VEC, INDEX)
#define __vector_first(VEC, TYPE) (TYPE *) vector_first(VEC)
#define __vector_last(VEC, TYPE) (TYPE *) vector_last(VEC)

void vector_init(vector *vec, size_t __basecap, size_t __elem_sz) {
    void *begin = malloc(__elem_sz * __basecap);
    iter_init(&vec->iterator, begin, __elem_sz, __basecap);
    vec->size = 0;
}

void vector_dstr(vector *vec) { free(vec->iterator.begin); }

void vector_ensure_capacity(vector *vec, size_t __new_cap) { iter_realloc(&vec->iterator, vec->iterator.len * 2); }

void vector_pushback(vector *vec, void *__value) {
    if (__value == NULL) {
        return;
    }

    // cap=count allocated links
    if (vec->size == vec->iterator.len) {
        vector_ensure_capacity(vec, vec->iterator.len * 2);
    }

    iter_set(&vec->iterator, vec->size);
    void *ic = iter_cur(&(vec->iterator));

    memcpy(ic, __value, vec->iterator.diff);
    vec->size++;
}

void *vector_at(vector *vec, size_t __index) {
    if (__index > vec->size) {
        return NULL;
    }

    iter_set(&vec->iterator, __index);
    return iter_cur(&vec->iterator);

    //return vec->buff + (sizeof(void *) * index);
}

void *vector_last(vector *vec) {
    if (vec->size == 0)
        return NULL;
    else {
        iter_set(&vec->iterator, vec->size - 1);
        return iter_cur(&vec->iterator);
    }
}

void *vector_first(vector *vec) {
    if (vec->size == 0)
        return NULL;
    else
        return vec->iterator.begin;
}

void vector_resize(vector *vec, size_t __newsz) { vec->size = __newsz; }

void vector_shrink_to_fit(vector *vec) {
    // delete elements by capacity
    //size_t new_size = sizeof(void *) * vec->size;

    iter_realloc(&vec->iterator, vec->size);
}

void vector_clear(vector *vec) {
    vector_resize(vec, 0);
    vector_shrink_to_fit(vec);
}

void vector_reserve(vector *vec, size_t count) {
    size_t free_now = (vec->iterator.len - vec->size);

    if (free_now < count) {
        vector_ensure_capacity(vec, count);
    }
}

#pragma endregion

// rm

int main() {
    printf("HI!\n");

    int v1 = 5;
    int v2 = 9;
    int v3 = -20;

    vector v;
    vector_init(&v, 2, sizeof(int));

    void *end = iter_end(&v.iterator);
    void *s = iter_begin(&v.iterator);

    printf("s=%p end=%p\n", s, end);

    vector_pushback(&v, &v1);
    vector_pushback(&v, &v2);

    int *cast = __vector_last(&v, int);
    printf("cast=%i\n", *cast);

    vector_dstr(&v);

    return 0;
}