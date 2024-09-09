
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*


    array
        -at
        -size
        -copy

    vector
    

    array
    -iterator()
    -
*/


void log(FILE *fd, const char *__msg, const char *__caller, const char *__file, const int __line) {
    fprintf(fd, "[ERR: %s:%i <%s>]: %s\n", __file, __line, __caller, __msg);
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

#pragma endregion

int main() {
    printf("HI!\n");

    int v1 = 5;

    array a;
    array_init(&a, 4, sizeof(int));
    array_set(&a, 3, &v1);

    int cur = *__array_at(&a, int, 3);
    printf("at=%i\n", cur);

    return 0;
}