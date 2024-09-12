
#include "../include/core/core.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h> // Для printf
#include <stdlib.h>
#include <string.h>
#include <string.h> // Для strdup

void log(FILE *fd, const char *__msg, const char *__caller, const char *__file, const int __line) {
    fprintf(fd, "[%s:%i <%s>]: %s\n", __file, __line, __caller, __msg);
}

// ::::::: Iterators
#pragma region Iterators

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

#pragma region Hashmap


#pragma endregion

#include <string.h>

// hashmap
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct hashmap_entry {
    const char *key;
    void *value;
    size_t _index;
} hashmap_entry;

typedef struct hashmap {
    hashmap_entry *entries;
    size_t capacity;
    size_t len;
} hashmap;

#define HASHMAP_INIT_CAP 16

void hashmap_init(hashmap *h) {
    h->len = 0;
    h->capacity = HASHMAP_INIT_CAP;
    h->entries = (hashmap_entry *) calloc(sizeof(hashmap_entry), h->capacity);
}

void hashmap_dstr(hashmap *h) {
    for (size_t i = 0; i < h->len; i++) {
        printf("FREE\n");
        free(h->entries[i].key);
    }

    free(h->entries);
}

unsigned int hash_key(const char *key) {
    unsigned int hash = FNV_OFFSET;
    for (const char *p = key; *p; p++) {
        hash ^= (unsigned int) (unsigned char) (*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void *hashmap_get(hashmap *map, const char *key) {
    unsigned int hash = hash_key(key);
    size_t start_index = (size_t) (hash & (unsigned int) (map->capacity - 1));

    printf("index=%i en=%p\n", start_index, map->entries);

    while (map->entries[start_index].key != NULL) // index < map.len
    {
        if (strcmp(map->entries[start_index].key, key) == 0) {
            return map->entries[start_index].value;
        } else {
            // linear probing
            start_index++;
            if (start_index >= map->capacity) {
                start_index = 0;
            }
        }
    }

    return NULL;
}

const char *hashmap_set_entry(hashmap *map, const char *key, void *value, size_t *plen) {
    // hashing key
    // check enty at the INDEX
    // probing next values

    unsigned int hash = hash_key(key);
    size_t index = (size_t) (hash & (unsigned int) (map->capacity - 1));

    printf("INDEX=%i\n", index);

    while (map->entries[index].key != NULL) {
        if (strcmp(map->entries[index].key, key) == 0) { // key is exists
            map->entries[index].value = value;
            return value;
        }

        index++;
        if (index >= map->capacity) {
            index = 0;
        }
    }

    // key not found.
    if (plen != NULL) {
        // fix
        int len = strlen(key);
        char *kbuff = (char *) malloc(sizeof(char) * (len + 1));
        kbuff[len] = '\0';
        strncpy(kbuff, key, len);
        key = kbuff;

        (*plen)++;
    }

    map->entries[index].key = (char *) key;
    map->entries[index].value = value;

    return key;
}

bool hashmap_expand(hashmap *map) {
    size_t new_cap = map->capacity * 2;

    printf("hs.expand\n");

    hashmap_entry *old = map->entries;
    size_t old_cap = map->capacity;
    size_t old_len = map->len;
    hashmap_entry *new = calloc(new_cap, sizeof(hashmap_entry));

    map->entries = new;
    map->capacity = new_cap;

    for (size_t i = 0; i < old_cap; i++) {
        hashmap_entry cur = old[i];
        if (cur.key != NULL) {
            hashmap_set_entry(map, cur.key, cur.value, NULL);
        }
    }

    free(old);
    return true;
}

void *hashmap_set(hashmap *map, const char *key, void *value) {
    if (value == NULL) {
        return NULL;
    }

    // ensure cap
    if (map->len >= (map->capacity / 2)) {
        // expand
        hashmap_expand(map);
    }

    return hashmap_set_entry(map, key, value, &map->len);
}

// rm

int main() {
    printf("HI!\n");


    int v1 = 5;
    int v2 = 9;
    int v3 = -20;

    hashmap map;
    hashmap_init(&map);

    void *g1 = hashmap_get(&map, "apple2");
    hashmap_set(&map, "apple", &v1);
    hashmap_set(&map, "apple2", &v1);
    hashmap_set(&map, "appl3", &v2);
    hashmap_set(&map, "appl4", &v1);
    hashmap_set(&map, "appl5", &v1);
    hashmap_set(&map, "appl6", &v3);
    hashmap_set(&map, "appl7", &v1);
    hashmap_set(&map, "appl8", &v1);
    hashmap_set(&map, "appl9", &v3);
    hashmap_set(&map, "apple10", &v1);

    printf("v1=%i\n", *(int *) hashmap_get(&map, "apple"));
    printf("v2=%i\n", *(int *) hashmap_get(&map, "apple2"));
    printf("v3=%i\n", *(int *) hashmap_get(&map, "appl3"));
    printf("v4=%i\n", *(int *) hashmap_get(&map, "appl4"));
    printf("v5=%i\n", *(int *) hashmap_get(&map, "appl5"));
    printf("v6=%i\n", *(int *) hashmap_get(&map, "appl6"));
    printf("v7=%i\n", *(int *) hashmap_get(&map, "appl7"));
    printf("v8=%i\n", *(int *) hashmap_get(&map, "appl8"));
    printf("v10=%i\n", *(int *) hashmap_get(&map, "apple10"));

    hashmap_dstr(&map);

    return 0;
}