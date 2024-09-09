#include "../include/http/request.h"

#include "../include/core/coredef.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

httprequest_buff *requestbuff_new(unsigned int cap) {
    httprequest_buff *buff = (httprequest_buff *) malloc(sizeof(httprequest_buff));
    buff->capacity = cap;
    buff->ptr = (char *) malloc(sizeof(char) * (cap + 1));
    memset(buff->ptr, 0, cap);
    buff->ptr[cap] = '\0';
    return buff;
};

void requestbuff_dstr(httprequest_buff *buff) { free(buff->ptr); };

void requestbuff_copy(const char *source, httprequest_buff *dest) {
    char t;
    int i = 0;
    while ((t = *source++) && i < dest->capacity) {
        dest->ptr[i] = t;
        i++;
    }

    dest->ptr[dest->capacity] = '\0';
};

void requestbuff_init(httprequest_buff *buff, unsigned int capacity) {
    buff->capacity = capacity;
    buff->ptr = (char *) malloc(sizeof(char) * capacity);
}

//
queryparam *queryparam_new(queryparam_value_type type, int value_size, int tag_name_size) {
    queryparam *q = (queryparam *) malloc(sizeof(queryparam));
    queryparam_init(q, type, value_size, tag_name_size);

    return q;
};

void queryparam_init(queryparam *param, queryparam_value_type type, int value_size, int tag_name_size) {
    param->type = type;
    param->tag_name = cstr_init(tag_name_size);
    if (type == QUERY_VALUE) {
        param->value.value = cstr_init(value_size);
    } else if (type == QUERY_VALUE_LIST) {
        param->value.list.value = cstr_init(value_size);
        param->value.list.next = NULL;
    }
};

char *queryparam_get_value(queryparam *qp) {
    if (!qp) {
        return NULL;
    }

    if (qp->type == QUERY_VALUE) {
        return qp->value.value;
    } else if (qp->type == QUERY_VALUE_LIST) {
        return qp->value.list.value;
    }
}