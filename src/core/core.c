
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

#pragma region Queue

typedef struct queue_entry {
    void *value;
    struct queue_entry *next;
} queue_entry;

typedef struct queue {
    // [] [] []
    // ll
    queue_entry *_head;
    queue_entry *_last;
    size_t _size;
    size_t _valsize;
} queue;

void queue_init(queue *q, size_t __valsize) {
    q->_last = NULL;
    q->_head = NULL;
    q->_size = 0;
    q->_valsize = __valsize;
}

void queue_push(queue *q, void *__valbuff) {
    if (__valbuff == NULL)
        return;

    queue_entry *en = (queue_entry *) malloc(sizeof(queue_entry));
    en->next = NULL;
    void *vb = (void *) malloc(q->_valsize);
    en->value = vb;
    memcpy(en->value, __valbuff, q->_valsize);

    if (q->_head != NULL) {
        q->_last->next = en;
        //printf("cur=%p\n", q->_last);
        q->_last = en;
        //printf("last, next=%p %p\n", q->_last, en);
    } else {
        q->_head = en;
        q->_last = en;
    }
    q->_size++;
}

void *queue_pop(queue *q) {
    if (q->_head == NULL) {
        return NULL;
    }

    queue_entry *first = q->_head;
    q->_head = q->_head->next;
    q->_size--;

    void *vl = first->value;

    free(first);

    return vl;
}

size_t queue_size(queue *q) { return q->_size; }

bool queue_empty(queue *q) { return q->_size == 0; }

void queue_clear(queue *q) {
    if (q->_head == NULL)
        return;

    queue_entry *head = q->_head;
    while (head != NULL) {
        queue_entry *next = head->next;
        printf("free: %p next=%p\n", head, next);
        free(head->value);
        free(head);
        head = next;
        q->_size--;
    }
}

void *queue_front(queue *q) { return q->_head->value; }

void *queue_back(queue *q) { return q->_last->value; }

#pragma endregion

typedef struct usr {
    int age;
    int value;
    char *name;
} usr;

usr *user_new(const char *name, int age) {
    usr *u1 = malloc(sizeof(usr));
    u1->age = age;
    u1->value = 0;
    u1->name = (char *) malloc(sizeof(char) * 6);
    strcpy(u1->name, name);
    return u1;
}

int vec_find(vector *vec, const char *name, int (*compr)(const char *a, void *b)) {
    for (size_t i = 0; i < vec->size; i++) {
        usr **elem = (usr **) vector_at(vec, i);
        if (elem != NULL) {
            if (compr(name, *elem) == 0) {
                return i;
            }
        }
    }

    return -1;
}

int compr_usr(const char *name, void *up) {
    usr *u = (usr *) up;
    if (strcmp(name, u->name) == 0)
        return 0;
    else
        return -1;
}

typedef int (*compr_base)(const char *a, void *b);

// comparators
int compr_ptr(void *ptra, void *ptrb) { return ptra == ptrb; }

int compr_dptr(void *dptr_a, void *dptr_b) {
    void *a = *(void **) dptr_a;
    void *b = *(void **) dptr_b;
    printf("A=%p B=%p\n", a, b);
    if (a == b) {
        return 0;
    }

    return -1;
}

// vector
typedef int (*basic_comparator)(void *a, void *b);
#define __basic_comparator int (*compare)(void *a, void *b)
/*
int trnode_compr_ptr(void *trnode_a, void *trnode_b) {
    tree_node *a = (tree_node *) trnode_a;
    tree_node *b = (tree_node *) trnode_b;

    if (a == NULL || b == NULL)
        return -1;

    if (a->value == b->value) {
        return 0;
    }

    return -1;
}
*/

#define __trnode_compr_default trnode_compr_ptr

int vector_find(vector *vec, void *elem, __basic_comparator) {
    for (size_t i = 0; i < vec->size; i++) {
        if (compare(elem, vector_at(vec, i)) == 0)
            return i;
    }

    return -1;
}

typedef struct tree_node {
    void *value;
    vector child_nodes;
} tree_node;

typedef struct tree {
    int value_size;
    int size;
    tree_node *head;
} tree;

// nodes
void trnode_init(tree *tr, tree_node *node, void *__value_buff) {
    vector_init(&node->child_nodes, 4, sizeof(tree_node *));
    if (__value_buff != NULL) {
        node->value = malloc(tr->value_size);
        memcpy(node->value, __value_buff, tr->value_size);
    } else
        node->value = NULL;
}

tree_node *trnode_new(tree *tr, void *__value_buff) {
    tree_node *node = (tree_node *) malloc(sizeof(tree_node));
    //printf("trnode_new=%p\n", node);
    trnode_init(tr, node, __value_buff);
    return node;
}

void trnode_dstr(tree_node *node) { free(node->value); }

void tree_init(tree *tree, size_t __valsize) {
    tree->value_size = __valsize;
    tree->head = (tree_node *) trnode_new(tree, NULL);
}

tree_node *tree_add(tree *tr, tree_node *node, void *__valbuff) {
    tr->size++;
    tree_node *nd = (tree_node *) trnode_new(tr, __valbuff);
    vector_pushback(&node->child_nodes, &(nd));

    return nd;
}

void tree_dump(tree *tr) {
    queue q;
    queue_init(&q, sizeof(tree_node *));
    queue_push(&q, &(tr->head));
    printf("queue.add=%p\n", tr->head);

    while (!queue_empty(&q)) {
        // add childs to queue
        tree_node **dptr = (tree_node **) queue_pop(&q);
        tree_node *head = NULL;
        if (dptr != NULL) {
            head = *dptr;
            //printf("free\n");
            free(dptr);
        }

        if (head->value != NULL) {
            printf("cur=%i\n", *(int *) head->value);
        }

        for (size_t i = 0; i < head->child_nodes.size; i++) {
            tree_node **find = (tree_node **) vector_at(&head->child_nodes, i);
            if (find != NULL) {
                queue_push(&q, find); //
            }
        }
    }

    queue_clear(&q);
}

void tree_dstr(tree *tr) {
    queue q;
    queue_init(&q, sizeof(tree_node *));
    queue_push(&q, &(tr->head));

    while (!queue_empty(&q)) {
        tree_node **dptr = (tree_node **) queue_pop(&q);
        tree_node *head = NULL;
        if (dptr != NULL) {
            head = *dptr;
            //printf("dptr.free=%p\n", dptr);
            free(dptr);
        } else
            break;

        if (head->value != NULL) {
            printf("cur=%i\n", *(int *) head->value);
        }

        for (size_t i = 0; i < head->child_nodes.size; i++) {
            tree_node **find = (tree_node **) vector_at(&head->child_nodes, i);
            if (find != NULL) {
                queue_push(&q, find); //
            }
        }

        //printf("free.head=%p\n", head->value);
        free(head->value);
        vector_dstr(&head->child_nodes);
        free(head);
    }


    queue_clear(&q);
}

int main() {
    printf("HI!\n");

    int t1 = 5;
    int t2 = 0;
    int t3 = 99;
    int t4 = -20;

    tree tr;
    tree_init(&tr, sizeof(int));
    tree_node *q1 = tree_add(&tr, tr.head, &t1);
    tree_node *q2 = tree_add(&tr, tr.head, &t2);
    tree_node *q3 = tree_add(&tr, tr.head, &t3);
    tree_node *q4 = tree_add(&tr, tr.head, &t4);

    tree_dump(&tr);
    tree_dstr(&tr);

    //int r1 = *(int *) queue_pop(&q);
    //printf("RES=%i\n", r1);

    return 0;
}

// void trnode_dstr_deep()
/*
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


int compr_usr_dptr(void *dptr_usra, void *dptr_usrb) {
    usr **t1 = (usr **) dptr_usra;
    usr **t2 = (usr **) dptr_usrb;
    if (t1 == NULL || t2 == NULL) {
        printf("compr_usr_dptr> bad cast. (dptr)\n");
        return -1;
    }

    if ((*t1)->value == (*t2)->value)
        return 0;

    return -1;
}

/*
    Queue
    -front()
    -back()
    -empty()
    -push
    -pop
    -new()
    -dstr()
    -clear()
*/
