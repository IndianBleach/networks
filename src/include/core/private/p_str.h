#ifndef _P_STR_H_
#define _P_STR_H_

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h> // Для printf
#include <stdlib.h>
#include <string.h>
#include <string.h> // Для strdup

#define __STR_CAPACITY_SHORT 32
#define __STR_CAPACITY_DEFAULT 64
#define __STR_CAPACITY_LONG 532
#define __STR_CAPACITY_LARGE 2048

struct string *new_string_initializer(int cap, struct string *f, ...);
struct string *new_string_initializer_ptr(int cap, const char *f, ...);

const char *_string_cstr(struct string *s);
struct string *_string_sub(struct string *s, int from, int len);
struct string *_string_sub_ptr(const char *s, int from, int len);
void _string_clear(struct string *s);
void _string_append(struct string *s, const char *val, size_t len);
void _string_append_s(struct string *s, struct string *append);
void _string_append_ptr(struct string *s, const char *append);
struct string *_string_dup(char *from);
struct string *_string_dup_s(struct string *s);

#define _StringCtor(f, cap, ...)                                                                                       \
    _Generic((f), struct string *: new_string_initializer, char *: new_string_initializer_ptr)(cap, NULL, __VA_ARGS__)

#define _StringParams(Capacity, First, ...) _StringCtor(First, Capacity, First, __VA_ARGS__)


#endif