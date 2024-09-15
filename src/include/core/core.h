#ifndef _CORE_H_
#define _CORE_H_

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h> // Для printf
#include <stdlib.h>
#include <string.h>
#include <string.h> // Для strdup

// ::::::: Logging
void log(FILE *fd, const char *__msg, const char *__caller, const char *__file, const int __line);
#define __log_err(msg) log(stderr, msg, __func__, __FILE__, __LINE__);
#define __log_info(msg) log(stdout, msg, __func__, __FILE__, __LINE__);

// ::::::: Iterators
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

void iter_init(basic_iterator *it, void *__begin, unsigned int __diff, size_t __len);
void iter_set(basic_iterator *it, size_t __pos);
void *iter_begin(basic_iterator *it);
void *iter_end(basic_iterator *it);
void *iter_cur(basic_iterator *it);
void *iter_next(basic_iterator *it);
void *iter_back(basic_iterator *it);
void *iter_realloc(basic_iterator *it, size_t _newcapacity);

// ::::::: Containers
typedef struct array {
    const basic_iterator iterator;
} array;

#define __array_at(ARR, TYPE, INDEX) (TYPE *) array_at(ARR, INDEX);

void array_init(array *arr, size_t cap, unsigned int elem_size);
size_t array_size(array *arr);
void *array_at(array *arr, size_t __index);
void array_set(array *arr, size_t __index, void *__value);
void array_dstr(array *arr);

typedef struct vector {
    size_t size;
    const basic_iterator iterator;
    // size + dynamic memory blocks
} vector;

#define __vector_at(VEC, TYPE, INDEX) (TYPE *) vector_at(VEC, INDEX)
#define __vector_first(VEC, TYPE) (TYPE *) vector_first(VEC)
#define __vector_last(VEC, TYPE) (TYPE *) vector_last(VEC)

void vector_init(vector *vec, size_t __basecap, size_t __elem_sz);
void vector_dstr(vector *vec);
void vector_ensure_capacity(vector *vec, size_t __new_cap);
void vector_pushback(vector *vec, void *__value);
void *vector_at(vector *vec, size_t __index);
void *vector_last(vector *vec);
void *vector_first(vector *vec);
void vector_resize(vector *vec, size_t __newsz);
void vector_shrink_to_fit(vector *vec);
void vector_clear(vector *vec);
void vector_reserve(vector *vec, size_t count);
void vector_dump(vector *vec);


#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#define HASHSET_INIT_CAP 16

typedef struct hashset_entry {
    char *key;
} hashset_entry;

typedef struct hashset {
    basic_iterator iterator;
    size_t size;
} hashset;

void hashset_init(hashset *set);
void hashset_dstr(hashset *set);
void hashset_add(hashset *set, char *key);
int hashset_get(hashset *set, const char *key);
void hashset_delete(hashset *set, const char *key);
char *hashset_at(hashset *set, size_t index);
void hashset_dump(hashset *set);
extern void hashset_ensure_capacity(hashset *set, size_t new_cap);


#define HASHMAP_INIT_CAP 16

typedef struct hashmap_entry {
    void *value;
    const char *key;
    int flags;
} hashmap_entry;

typedef struct hashmap {
    basic_iterator iterator;
    size_t size;
    size_t type_sz;
} hashmap;

void hashmap_init(hashmap *map, int type_sz);
void hashmap_dstr(hashmap *map);
void hashmap_add(hashmap *map, const char *__key, void *__value_buff);
int hashmap_get_index(hashmap *map, const char *__key);
void *hashmap_get(hashmap *map, const char *__key);
void *hashmap_at(hashmap *map, size_t index);
void hashmap_clear(hashmap *map);
void hashmap_dump(hashmap *map);
void hashmap_ensure_capacity(hashmap *map, size_t new_cap);


// ::::::: DS

// tree
/*

sorted_map
sorted_set
sorted_tree

TREE
    clear(tree)
    dstr(tree)
    new(headsz)
    init(tree, headsz)
    
    clear_from(node)
    bfs(tree, val_comparator)
    dfs(tree, val_comparator)

    // vec_comparator

TREE_NODE
    void* value
    vector<TREE_NODE> childrens
*/

// size
// tree_node
// compr(node* node, void* node_value)
// dfs(node* root, compr*)
// bfs(node* root, compr*)

#endif