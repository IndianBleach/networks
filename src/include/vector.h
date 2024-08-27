#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t len;
    size_t cap;
    size_t type_sz;
    void *data;
} vector;

extern void vector_init(vector *v, size_t type_sz, size_t init_cap);
extern void vector_destroy(vector *v);
extern int vector_ensure_capacity(vector *v, size_t need);

extern void vector_add_at(vector *v, size_t at);
extern void vector_push_back(vector *v, void *value);

extern void *vector_get(vector *v, size_t at);

extern void vector_rmv_at(vector *v, size_t at);
extern void vector_rmv_last(vector *v);
extern int vector_shrink(vector *v);
