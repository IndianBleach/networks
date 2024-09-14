
#include "../include/core/core.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h> // Для printf
#include <stdlib.h>
#include <string.h>

// ::::::: Strings
char *cstrdup(const char *str) {
    size_t len = strlen(str);
    char *buff = (char *) malloc(sizeof(char) * (len + 1));
    buff[len] = '\0';
    strncpy(buff, str, len);

    return buff;
}

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
    memset(it->begin, 0, it->len * it->diff);
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
    memset(begin, 0, __elem_sz * __basecap);
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

void vector_dump(vector *vec) {
    for (size_t i = 0; i < vec->size; i++) {
        printf("vec[%i]: %p\n", i, vector_at(vec, i));
    }
}

#pragma endregion

#pragma region HashSet

unsigned int hash_key(const char *key) {
    unsigned int hash = FNV_OFFSET;
    for (const char *p = key; *p; p++) {
        hash ^= (unsigned int) (unsigned char) (*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void hashset_init(hashset *set) {
    void *buff = (void *) calloc(sizeof(hashset_entry), HASHSET_INIT_CAP);
    iter_init(&set->iterator, buff, sizeof(hashset_entry), HASHSET_INIT_CAP);

    set->size = 0;
}

void hashset_dstr(hashset *set) {
    hashset_entry *buff = (hashset_entry *) set->iterator.begin;
    for (int i = 0; i < set->iterator.len; i++) {
        free(buff[i].key);
    }

    free(set->iterator.begin);
}

void hashset_add(hashset *set, char *key) {
    // get hash
    // get index
    // check at the index
    // insert if no ex

    unsigned int hash = hash_key(key);
    size_t index = (size_t) (hash & (unsigned int) (set->iterator.len - 1));

    //printf("INDEX=%i BUFF=%p LEN=%i\n", index, set->iterator.begin, set->iterator.len);

    hashset_entry *buff = (hashset_entry *) set->iterator.begin;
    while (buff[index].key != NULL) {
        if (strcmp(buff[index].key, key) == 0) {
            return;
        } else {
            index++;
            if (index >= set->iterator.len) {
                index = 0;
            }
        }
    }

    // not found. insert
    // string

    buff[index].key = cstrdup(key);
    set->size++;

    if (set->size >= (set->iterator.len / 2)) {
        hashset_ensure_capacity(set, set->iterator.len * 2);
    }
}

int hashset_get(hashset *set, const char *key) {
    unsigned int hash = hash_key(key);
    size_t index = (size_t) (hash & (unsigned int) (set->iterator.len - 1));

    hashset_entry *buff = (hashset_entry *) set->iterator.begin;
    while (buff[index].key != NULL) {
        //printf("CUR=%i\n", index);
        if (strcmp(buff[index].key, key) == 0) {
            return index;
        } else {
            index++;
            if (index >= set->iterator.len) {
                index = 0;
            }
        }
    }

    return -1;
}

void hashset_ensure_capacity(hashset *set, size_t new_cap) {
    // delete old buff
    // new buff and recopy elements

    hashset_entry *old_buff = set->iterator.begin;
    size_t old_cap = set->iterator.len;

    void *buff = (void *) calloc(sizeof(hashset_entry), new_cap);
    set->iterator.begin = buff;
    set->iterator.len = new_cap;
    set->size = 0;

    // copy elems
    for (size_t i = 0; i < old_cap; i++) {
        //printf("I=%i\n", i);
        if (old_buff[i].key != NULL) {
            hashset_add(set, old_buff[i].key);
            free(old_buff[i].key);
        }
    }

    free(old_buff);
}

void hashset_delete(hashset *set, const char *key) {
    int index = hashset_get(set, key);
    if (index > 0) {
        iter_set(&set->iterator, index);
        hashset_entry *item = (hashset_entry *) iter_cur(&set->iterator);
        //printf("GETTED=%s %i\n", item->key, index);
        memset(item->key, NULL, strlen(item->key));
        set->size--;
    }
}

char *hashset_at(hashset *set, size_t index) {
    if (index >= set->iterator.len)
        return NULL;
    iter_set(&set->iterator, index);
    hashset_entry *elem = (hashset_entry *) iter_cur(&set->iterator);

    return elem->key;
}

void hashset_dump(hashset *set) {
    //printf("hashset_dump: i=0 len=%i\n", set->iterator.len);
    for (size_t i = 0; i < set->iterator.len; i++) {
        printf("set[%i]= %s\n", i, hashset_at(set, i));
    }
}

// init
// new
// _ensure_cap
// add(key)
// delete(key)
// contains(key)

#pragma endregion

#pragma region Hashmap

void hashmap_init(hashmap *map, int type_sz) {
    void *buff = calloc(sizeof(hashmap_entry), HASHMAP_INIT_CAP);
    iter_init(&map->iterator, buff, sizeof(hashmap_entry), HASHMAP_INIT_CAP);
    map->size = 0;
    map->type_sz = type_sz;
}

void hashmap_add(hashmap *map, const char *__key, void *__value_buff) {
    if (hashmap_get_index(map, __key) > 0) {
        // exists
        printf("exi: %s\n", __key);
        return;
    } else {
        unsigned int hash = hash_key(__key);
        size_t index = (size_t) (hash & (unsigned int) (map->iterator.len - 1));

        hashmap_entry *buff = (hashmap_entry *) map->iterator.begin;

        // while, linear probbing
        while (buff[index].flags != 0) {
            index++;
            if (index >= map->iterator.len) {
                index = 0;
            }
        }

        buff[index].key = cstrdup(__key);

        // alloc new memorey for value
        // memcpy to the buffer
        buff[index].value = malloc(map->type_sz);
        memcpy(buff[index].value, __value_buff, map->type_sz);
        buff[index].flags = 1;
        map->size++;

        //printf("sz=%i\n", map->size);
        if (map->size >= (map->iterator.len / 2)) {
            hashmap_ensure_capacity(map, map->iterator.len * 2);
        }
    }
}

int hashmap_get_index(hashmap *map, const char *__key) {
    unsigned int hash = hash_key(__key);
    size_t index = (size_t) (hash & (unsigned int) (map->iterator.len - 1));

    hashmap_entry *buff = (hashmap_entry *) map->iterator.begin;
    while (buff[index].flags != 0 && buff[index].key != NULL) {
        if (strcmp(buff[index].key, __key) == 0) {
            // key found
            return index;
        }
    }

    return -1;
}

void *hashmap_get(hashmap *map, const char *__key) {
    int find = hashmap_get_index(map, __key);
    if (find == -1)
        return NULL;

    hashmap_entry *buff = (hashmap_entry *) map->iterator.begin;
    return buff[find].value;
}

void hashmap_ensure_capacity(hashmap *map, size_t new_cap) {
    // delete old buff
    // new buff and recopy elements

    hashmap_entry *old_buff = (hashmap_entry *) map->iterator.begin;
    size_t old_cap = map->iterator.len;

    void *buff = (void *) calloc(sizeof(hashmap_entry), new_cap);
    map->iterator.begin = buff;
    map->iterator.len = new_cap;
    map->size = 0;

    //printf("hashmap_ensure_capacity: old=%p new=%p cap=%i\n", old_buff, buff, new_cap);

    // copy elems
    for (size_t i = 0; i < old_cap; i++) {
        //printf("I=%i\n", i);
        if (old_buff[i].key != NULL) {
            hashmap_add(map, old_buff[i].key, old_buff[i].value);

            free(old_buff[i].key);
            free(old_buff[i].value);
        }
    }

    free(old_buff);
}

void hashmap_clear(hashmap *map) {
    hashmap_entry *buff = (hashmap_entry *) map->iterator.begin;
    for (size_t i = 0; i < map->iterator.len; i++) {
        if (buff[i].flags != 0 && buff[i].key != NULL) {
            printf("free: %s\n", buff[i].key);
            free(buff[i].key);
            free(buff[i].value);
            buff[i].flags = 0;
        }
    }

    map->size = 0;
    //free(map->iterator.begin);
}

void *hashmap_at(hashmap *map, size_t index) {
    if (index >= map->iterator.len)
        return NULL;
    iter_set(&map->iterator, index);
    hashmap_entry *elem = (hashmap_entry *) iter_cur(&map->iterator);

    return elem->key;
}

void hashmap_dump(hashmap *map) {
    //printf("hashset_dump: i=0 len=%i\n", set->iterator.len);
    for (size_t i = 0; i < map->iterator.len; i++) {
        hashmap_entry *en = hashmap_at(map, i);
        printf("map[%i]=  val=%p\n", i, en);
    }
}

void hashmap_dstr(hashmap *map) {
    printf("hashmap_dstr\n");

    hashmap_entry *buff = (hashmap_entry *) map->iterator.begin;

    for (size_t i = 0; i < map->iterator.len; i++) {
        if (buff[i].flags != 0 && buff[i].key != NULL) {
            printf("__hashmap_dstr[%s]\n", buff[i].key);
            free(buff[i].value);
            free(buff[i].key);
        }
    }

    free(map->iterator.begin);
}

#pragma endregion

typedef struct usr {
    int age;
    int value;
    char *name;
} usr;

usr *user_new() {
    usr *u1 = malloc(sizeof(usr));
    u1->age = 12;
    u1->value = 200;
    u1->name = (char *) malloc(sizeof(char) * 6);
    strcpy(u1->name, "apple");
    return u1;
}

typedef union usr2 {
    int d;
    vector vec;
    int t;
} usr2;

/*
int main() {
    printf("HI!\n");

    usr2 t;
    t.d = 5;
    printf("t=%i\n", t.d);
    t.t = -10;
    printf("t=%i\n", t.d);
    t.vec.size = 1045;
    printf("t=%i\n", t.d);

    return 0;
}*/
