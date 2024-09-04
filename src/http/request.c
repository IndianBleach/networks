#include "../include/http/request.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

httpreq_buff *reqbuff_new(unsigned int cap) {
    httpreq_buff *buff = (httpreq_buff *) malloc(sizeof(httpreq_buff));
    buff->capacity = cap;
    buff->ptr = (char *) malloc(sizeof(char) * (cap + 1));
    memset(buff->ptr, 0, cap);
    buff->ptr[cap] = '\0';
    return buff;
};

void reqbuff_dstr(httpreq_buff *buff) {
    free(buff->ptr);
    free(buff);
};

void reqbuff_copy(const char *source, httpreq_buff *dest) {
    char t;
    int i = 0;
    while ((t = *source++) && i < dest->capacity) {
        dest->ptr[i] = t;
        i++;
    }

    dest->ptr[dest->capacity] = '\0';
}