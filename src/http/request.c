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
    p->value.type = UNSET;
    p->name = NULL;
    return p;
}

void _dump_nested_list(tree *tr) {
    queue q;
    queue_init(&q, sizeof(tree_node *));
    queue_push(&q, &(tr->head));
    //printf("[_dump_nested_list] head=%p\n", tr->head);

    while (!queue_empty(&q)) {
        // add childs to queue
        tree_node **dptr = (tree_node **) queue_pop(&q);
        tree_node *head = NULL;
        if (dptr != NULL) {
            head = *dptr;
            //printf("free\n");
            free(dptr);
        }

        //printf("HEAD=%p %p\n", head, head->value);
        if (head->value != NULL) {
            header_value *v1 = *(header_value **) head->value;
            //printf("VV1=%i\n", v1->type);
            switch (v1->type) {
                case HVAL_WORD:
                    printf("[tree] word=%s\n", v1->single_value);
                    break;

                case HVAL_LIST:
                    //printf("[LIST] size=%i\n", v1->nested_list->head->child_nodes.size);
                    for (size_t i = 0; i < v1->nested_list->head->child_nodes.size; i++) {
                        tree_node **find = (tree_node **) vector_at(&v1->nested_list->head->child_nodes, i);
                        if (find != NULL) {
                            //printf("push2=%p\n", find);
                            queue_push(&q, find); //
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        for (size_t i = 0; i < head->child_nodes.size; i++) {
            tree_node **find = (tree_node **) vector_at(&head->child_nodes, i);
            if (find != NULL) {
                //printf("push=%p\n", find);
                queue_push(&q, find); //
            }
        }
    }

    queue_clear(&q);
}

void httpheader_dump(httpheader *t) {
    if (t == NULL) {
        printf("DUMP.httpheader: error>null\n");
        return;
    }

    if (t->value.type == HVAL_TAGVALUE) {
        printf("DUMP.httpheader: name=%s type=TAGVAL [tag=%s val=%s]\n", t->name, t->value.tag_value.tag,
               t->value.tag_value.value);
    } else if (t->value.type == HVAL_LIST) {
        printf("DUMP.httpheader: name=%s type=NEST_TREE: %p\n", t->name, t->value.nested_list);
        _dump_nested_list(t->value.nested_list);
    } else if (t->value.type == UNSET) {
        printf("DUMP.httpheader: name=%s type=UNSET val=%s\n", t->name);
    } else {
        printf("DUMP.httpheader: name=%s type=%i val=%s\n", t->name, t->value.type, t->value.single_value);
    }
}
