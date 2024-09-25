#ifndef _CORE_STR_H
#define _CORE_STR_H

#include "../include/core/private/p_str.h"

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h> // Для printf
#include <stdlib.h>
#include <string.h>

struct string;

typedef struct string string;

#define String_dstr(__str) __string_dstr(__str);

#define String_fdstr(__str)                                                                                            \
    __string_dstr(__str);                                                                                              \
    free(__str)

#define String(...) _StringParams(__STR_CAPACITY_DEFAULT, __VA_ARGS__, NULL)
#define StringShort(...) _StringParams(__STR_CAPACITY_SHORT, __VA_ARGS__, NULL)
#define StringLong(...) _StringParams(__STR_CAPACITY_LONG, __VA_ARGS__, NULL)
#define StringLarge(...) _StringParams(__STR_CAPACITY_LARGE, __VA_ARGS__, NULL)

#define String_dup(_str) _Generic((_str), string *: _string_dup_s, char *: _string_dup)(_str)

#define String_clear(__str) _string_clear(__str)

#define String_c_str(_str) _string_cstr(_str)

#define String_set_len(_str, len) _string_set_len(_str, len)

#define String_sub(_str, from, len)                                                                                    \
    _Generic((_str),\ 
    string *: _string_sub, \                                                                                             
    char *: _string_sub_ptr \
)(_str, from, len)

#define String_append(_self_ptr, value)                                                                                \
    _Generic((value), char *: _string_append_ptr, string *: _string_append_s)(_self_ptr, value)

#define String_get_buff(__str) _string_get_buff(__str)

#endif