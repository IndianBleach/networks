
#include "../include/core/core.h"

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h> // Для printf
#include <stdlib.h>
#include <string.h>

// ::::::: String
char *itoa(int num) {
    int diff = 1;

    int len = 0;
    int t = num;
    while (t > 0) {
        t /= 10;
        len++;
    }

    char *buff = malloc(sizeof(char) * (len + 1));
    buff[len] = '\0';

    while (num > 0) {
        diff = num % 10;
        num = (num - diff) / 10;
        buff[--len] = diff + '0';
    }

    return buff;
}

char *substr(char *buff, int len) {
    char *s = malloc(sizeof(char) * (len + 1));
    s[len] = '\0';

    if (strlen(buff) < len) {
        return NULL;
    }

    for (int i = 0; i < len; i++) {
        s[i] = buff[i];
    }


    return s;
}

// ::::::: Comparators

int __comparator_ptr(void *ptra, void *ptrb) { return ptra == ptrb; }

int __comparator_dptr(void *dptra, void *dptrb) {
    void *a = *(void **) dptra;
    void *b = *(void **) dptrb;
    //printf("A=%p B=%p\n", a, b);
    if (a == b) {
        return 0;
    }

    return -1;
}

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
            //printf("free: %s\n", buff[i].key);
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
    //printf("hashmap_dstr\n");

    hashmap_entry *buff = (hashmap_entry *) map->iterator.begin;

    for (size_t i = 0; i < map->iterator.len; i++) {
        if (buff[i].flags != 0 && buff[i].key != NULL) {
            //printf("__hashmap_dstr[%s]\n", buff[i].key);
            free(buff[i].value);
            free(buff[i].key);
        }
    }

    free(map->iterator.begin);
}

#pragma endregion

#pragma region Queue

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

#pragma region Tree

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
    printf("[tree_dump] head=%p\n", tr->head);

    while (!queue_empty(&q)) {
        // add childs to queue
        tree_node **dptr = (tree_node **) queue_pop(&q);
        tree_node *head = NULL;
        if (dptr != NULL) {
            head = *dptr;
            //printf("free\n");
            free(dptr);
        }

        printf("DUMP.tree: node=%p\n", head);

        if (head->value != NULL) {
            //printf("cur=%i\n", *(int *) head->value);
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

#pragma endregion

typedef struct stack_entry {
    void *value;
    struct stack_entry *prev;
} stack_entry;

typedef struct stack {
    stack_entry *head;
    size_t typesize;
    size_t len;
} stack;

void stack_entry_init(stack_entry *en, void *__value, size_t __typesize) {
    en->prev = NULL;
    if (__value != NULL) {
        void *buff = malloc(__typesize);
        en->value = buff;
        memcpy(en->value, __value, __typesize);
    }
}

stack_entry *stack_entry_new(void *__value, size_t __typesize) {
    stack_entry *v = (stack_entry *) malloc(sizeof(stack_entry));
    stack_entry_init(v, __value, __typesize);

    return v;
}

void stack_init(stack *s, size_t __typesize) {
    s->typesize = __typesize;
    s->head = NULL;
    s->len = 0;
}

int stack_push(stack *s, void *__value) {
    if (s->head != NULL) {
        stack_entry *old = s->head;
        s->head = (stack_entry *) stack_entry_new(__value, s->typesize);
        s->head->prev = old;
    } else {
        s->head = (stack_entry *) stack_entry_new(__value, s->typesize);
        s->head->prev = NULL;
    }
    s->len++;
}

void *stack_pop(stack *s) {
    if (s->head != NULL) {
        stack_entry *old = s->head;
        s->head = s->head->prev;
        s->len--;
        void *r = old->value;
        free(old);
        return r;
    } else
        return NULL;
}

int stack_size(stack *s) { return s->len; }

void *stack_top(stack *s) {
    if (s->head != NULL) {
        return s->head->value;
    } else
        return NULL;
}

bool stack_empty(stack *s) {
    //printf("len=%i\n", s->len);
    return s->len == 0;
}

void stack_dstr(stack *s) {
    stack_entry *en = s->head;
    while (en != NULL) {
        stack_entry *c = en;
        en = en->prev;
        free(c->value);
        free(c);
    }
}

/*
empty	Проверяет, является ли stack пустым.
pop	Удаляет элемент из верхней части stack.
push	Добавляет элемент в верхнюю часть stack.
size	Возвращает количество элементов в контейнере stack.
top	Возвращает ссылку на элемент в верхней части stack.
*/


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

// queue -> add . par
bool tree_bfs_contains(tree *tr, __basic_comparator, void *__value) {
    queue q;
    queue_init(&q, sizeof(tree_node *));
    queue_push(&q, &(tr->head));

    while (!queue_empty(&q)) {
        tree_node **dptr = queue_pop(&q);
        if (dptr != NULL) {
            tree_node *node = *dptr;
            printf("node=%p\n", node);

            if (node->value != NULL) {
                if (compare(node->value, (__value)) == 0) {
                    return true;
                }
            }

            for (size_t i = 0; i < node->child_nodes.size; i++) {
                queue_push(&q, vector_at(&node->child_nodes, i));
            }
        }

        free(dptr);
    }

    queue_clear(&q);

    return false;
}

bool tree_dfs_contains(tree *tr, __basic_comparator, void *__value) {
    /*
        stack s
        s.push(head)

        while (!s.empty())
            cur = s.pop()
            comparing..
            visits.add(cur)

            for(i in cur.childs)
                if (cur.childs[i].visit == false)
                    s.push(cur.childs[i])
    */

    hashset set;
    hashset_init(&set);

    stack s;
    stack_init(&s, sizeof(tree_node *));
    stack_push(&s, &(tr->head));

    bool res = false;

    while (!stack_empty(&s)) {
        tree_node **dptr = stack_pop(&s);
        if (dptr != NULL) {
            tree_node *node = *dptr;

            char *hash = itoa((int) node);

            if (node->value != NULL) {
                if (compare(node->value, __value) == 0)
                    res = true;
            }

            for (size_t i = 0; i < node->child_nodes.size; i++) {
                if (hashset_get(&set, hash) == -1) {
                    stack_push(&s, vector_at(&node->child_nodes, i));
                }
            }

            free(hash);
        }

        free(dptr);
    }

    stack_dstr(&s);
    hashset_dstr(&set);

    return res;
}

int compr_ptrval_int(void *dptra, void *dptrb) {
    int *f1 = (int *) dptra;
    int *f2 = (int *) dptrb;
    if (*f1 == *f2)
        return 0;

    return -1;
}

#include "../include/core/str.h"
#include "../include/http/httpinfo.h"
#include "../include/http/request.h"

/*
    httpResponse
        statusCode
        buff

    response_new(code, gen_response(request))

*/

typedef char *httpresponse_buffptr;

typedef struct httpresponse {
    http_statuscode code;
    httpresponse_buffptr buff;
} httpresponse;

void gen_response(httprequest *__request, httpresponse *response) {
    // not found pages
    // auth pages
    //

    if (__request->method == HTTP_GET) {
        // filePage
    } else {
        http_statuscode code;
        char *method;
    }
}
