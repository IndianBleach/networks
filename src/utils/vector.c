#include "vector.h"

#include <string.h>

void vector_init(vector *v, size_t sz, size_t init_cap) {
    assert(v);

    v->len = 0;
    v->type_sz = sz;
    v->cap = init_cap;

    v->data = malloc(v->cap * v->type_sz);
    if (!v->data) {
        fprintf(stderr, "Heap allocation error\n");
        return;
    }
}

void vector_destroy(vector *v) {
    free(v->data);
    v->data = NULL;
}

int vector_ensure_capacity(vector *v, size_t need) {
    size_t nlen = v->len + need;
    size_t ncap = nlen * 2;
    if (nlen >= v->cap) {
        v->data = realloc(v->data, ncap * v->type_sz);
        if (!v->data) {
            fprintf(stderr, "Heap allocation error\n");
            return 1;
        }
        v->cap = ncap;
    }
    v->len = nlen;
    return 0;
}

void vector_push_back(vector *v, void *value) {
    int err = vector_ensure_capacity(v, 1);
    if (err || !value)
        return;
    size_t offset = (v->len - 1) * v->type_sz;
    memcpy((char *) v->data + offset, value, v->type_sz);
}

void *vector_get(vector *v, size_t at) {
    if (at >= v->len)
        return NULL;
    return ((char *) v->data) + (at * v->type_sz);
}

int vector_shrink(vector *v) {
    size_t nlen = v->len - 1;
    size_t ncap = v->cap / 2;
    if (nlen <= v->cap / 4) {
        v->data = realloc(v->data, ncap * v->type_sz);
        if (!v->data) {
            fprintf(stderr, "Heap allocation error\n");
            return 1;
        }
        v->cap = ncap;
    }
    return 0;
}
void vector_rmv_at(vector *v, size_t at) {
    // TODO
    return;
}

void vector_rmv_last(vector *v) {
    if (v->len == 0)
        return;
    int err = vector_shrink(v);
    if (err) {
        fprintf(stderr, "Some error from vector_shrink()\n");
        return;
    }
    v->len--;
}
