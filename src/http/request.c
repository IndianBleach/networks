#include "../include/http/request.h"

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