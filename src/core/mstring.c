
#include "../include/core/core.h"
#include "../include/core/private/p_str.h"
#include "../include/core/str.h"

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h> // Для printf
#include <stdlib.h>
#include <string.h>

typedef struct string {
    basic_iterator iterator;
    size_t len;
} string;

void _string_init(string *s, size_t cap) {
    assert(cap > 0);
    char *buff = (char *) malloc(cap + 1);
    buff[cap] = '\0';
    iter_init(&s->iterator, buff, sizeof(char), cap);
    s->len = 0;
};

void _string_init2(string *s, size_t cap, const char *val) {
    assert(cap > 0);
    char *buff = (char *) malloc(cap + 1);
    buff[cap] = '\0';
    iter_init(&s->iterator, buff, sizeof(char), cap);

    size_t sz = strlen(val);

    strncpy(s->iterator.begin, val, sz);
    s->len = sz;
};

string *_string_new_empty(size_t cap) {
    string *s = (string *) malloc(sizeof(string));
    _string_init(s, cap);
    return s;
}

string *_string_new(size_t cap, const char *val) {
    string *s = (string *) malloc(sizeof(string));
    _string_init2(s, cap, val);
    return s;
}

void __string_dstr(string *str) { free(str->iterator.begin); }

void _string_ensure_capacity(string *s, size_t ncap) {
    char *old = s->iterator.begin;
    s->iterator.begin = (char *) realloc(old, (ncap + 1) * sizeof(char));
    s->iterator.len = ncap;
    ((char *) s->iterator.begin)[ncap] = '\0';
    // clearing default trash values
    memset(&s->iterator.begin[s->len], 0, (ncap + 1 - s->len) * sizeof(char));
}

// initializers
string *new_string_initializer(int cap, string *f, ...) {
    // new string
    string *res = _string_new_empty(cap);

    va_list ls;
    va_start(ls, 0);
    while (true) {
        string *p;
        if (p = va_arg(ls, string *)) {
            String_append(res, p);
        } else
            break;
    }

    va_end(ls);

    return res;
};

string *new_string_initializer_ptr(int cap, const char *f, ...) {
    // new string
    string *res = _string_new_empty(cap);

    va_list ls;
    va_start(ls, 0);
    while (true) {
        const char *p;
        if (p = va_arg(ls, const char *)) {
            //printf("[str_init_ptr]=%s %i\n", p, cap); // append
            String_append(res, (char *) p);
        } else
            break;
    }

    va_end(ls);

    return res;
};

char *_string_get_buff(struct string *s) { return (char *) s->iterator.begin; }

void _string_set_len(struct string *s, int len) { s->len = len; }

// c_str
const char *_string_cstr(string *s) { return (const char *) s->iterator.begin; }

// substring
string *_string_sub(string *s, int from, int len) {
    int sz = len;
    if (from + len > s->len)
        sz = s->len;

    assert(from < s->len);

    string *v = _string_new_empty(sz);
    strncpy(v->iterator.begin, &s->iterator.begin[from], sz);

    return v;
}

string *_string_sub_ptr(const char *s, int from, int len) {
    int sz = len;

    int source_sz = strlen(s);

    if (from + len > source_sz)
        sz = source_sz;

    assert(from < source_sz);

    string *v = _string_new_empty(sz);
    strncpy(v->iterator.begin, &s[from], sz);

    return v;
};

// clear
void _string_clear(string *s) { memset(s->iterator.begin, 0, s->len); }

// append
void _string_append(string *s, const char *val, size_t len) {
    if (s->len + len >= s->iterator.len) {
        _string_ensure_capacity(s, s->iterator.len * 2);
    }

    // fix: need more capacity than current x2.

    strncpy(&s->iterator.begin[s->len], val, len);
    s->len += len;
}

void _string_append_s(string *s, string *append) {
    _string_append(s, (const char *) append->iterator.begin, append->len);
};

void _string_append_ptr(string *s, const char *append) { _string_append(s, append, strlen(append)); };

// dup
string *_string_dup(char *from) {
    int sz = strlen(from);
    assert(sz > 0);

    string *s = _string_new_empty(sz);

    memcpy(s->iterator.begin, from, sz);

    return s;
}

string *_string_dup_s(string *s) { return _string_dup((char *) s->iterator.begin); }
