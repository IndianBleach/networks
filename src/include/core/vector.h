#ifndef _CORE_VECTOR_
#define _CORE_VECTOR_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vector {
    void *buff;
    size_t capacity;
    size_t size;
    size_t type_size;
} vector;

void vector_init(vector *vec, size_t base_cap, size_t type_sz);

void vector_dstr(vector *vec);

void vector_ensure_capacity(vector *vec, size_t new_cap);

void vector_push_back(vector *vec, void *elem);

void *vector_at(vector *vec, size_t index);

void vector_resize(vector *vec, size_t new_size);

void vector_shrink_to_fit(vector *vec);

void vector_clear(vector *vec);

void vector_reserve(vector *vec, size_t count);


#endif