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
    buff->ptr = (char *) malloc(sizeof(char) * (capacity + 1));
}

void queryparam_init(queryparam *param, queryparam_value_type type, char *tag_name, char *tag_value) {
    param->type = type;
    if (type == QUERY_VALUE) {
        param->tag_name = tag_name;
        param->value.value = tag_value;
        printf("sss=%s\n", param->value.value);
    } else if (type == QUERY_VALUE_LIST) {
        param->value.value = NULL;
        vector_init(&param->value.vec, 10, sizeof(char *));
        vector_pushback(&param->value.vec, tag_value);
    }
};

void queryparam_switch_on_list(queryparam *param, queryparam_value_type type) {
    if (param->type == QUERY_VALUE && type == QUERY_VALUE_LIST) {
        // change from single value TO LIST
        printf("%s\n", param->value.value);
        char *old = param->value.value;
        param->type = QUERY_VALUE_LIST;
        //param->value.value = NULL;
        //param->value.vec.size = 0;
        vector_init(&param->value.vec, 10, sizeof(char *));
        //vector_dump(&param->value.vec);
        vector_pushback(&param->value.vec, &(old));
        //param->value.value = NULL; // set old value NULL;
    }
}

// :: Headers
httpheader *httpheader_new(header_value_type __base_type) {
    httpheader *p = (httpheader *) malloc(sizeof(httpheader));
    p->type = __base_type;
    p->name = NULL;
    return p;
}

void httpheader_dump(httpheader *t) {
    if (t == NULL) {
        printf("dump.httpheader: error>null\n");
        return;
    }

    if (t->type == HEADER_SINGLE_VALUE) {
        printf("dump.httpheader: name=%s type=SINGLE val=%s\n", t->name, t->value.single_value);
    } else if (t->type == HEADER_TAGVALUE) {
        printf("dump.httpheader: name=%s type=TAGVAL [tag=%s val=%s]\n", t->name, t->value.tag_value.tag,
               t->value.tag_value.value);
    } else if (t->type == HEADER_NESTED_TREE) {
        printf("dump.httpheader: name=%s type=NEST_TREE:\n", t->name);
        tree_dump(t->value.nested_list);
    }
}
