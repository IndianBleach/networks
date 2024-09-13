
#include "../include/core/core.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h> // Для printf
#include <stdlib.h>
#include <string.h>
#include <string.h> // Для strdup

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

#pragma region HashSet

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define HASHSET_INIT_CAP 4

typedef struct hashset_entry {
    char *key;
} hashset_entry;

typedef struct hashset {
    basic_iterator iterator;
    size_t size;
} hashset;

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

void hashset_dstr(hashset *set) { free(set->iterator.begin); }

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

#include <string.h>

// hashmap

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

    hashset set;
    hashset_init(&set);
    hashset_add(&set, "apple");
    hashset_add(&set, "apple2");
    hashset_add(&set, "banana-expadqwdqwdw");
    hashset_add(&set, "banana-expadqw123dqwdw");
    hashset_add(&set, "banana-qwd");
    hashset_add(&set, "banana-123");

    printf("t1refwefwe\n");

    int t11 = hashset_get(&set, "apple2");
    hashset_delete(&set, "apple2");

    hashset_add(&set, "apple2");

    int t1 = hashset_get(&set, "apple");
    int t2 = hashset_get(&set, "apple2");
    int t3 = hashset_get(&set, "banana3");


    hashset_dump(&set);

    return 0;
}