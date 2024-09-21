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

void queryparam_init(queryparam *param, queryparam_value_type type, char **tag_name, char **tag_value) {
    param->type = type;
    if (type == QUERY_VALUE) {
        param->tag_name = *tag_name;
        param->value.value = *tag_value;
        //printf("sss=%s\n", param->value.value);
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
                case HVAL_STRING:
                case HVAL_INT:
                case HVAL_FLOAT:
                case HVAL_VERSION:
                case HVAL_PATH:
                    printf("-->%s ", v1->single_value);
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
    printf("\n");
}

void httpheader_dump(httpheader *t) {
    if (t == NULL) {
        printf("DUMP.httpheader: error>null\n");
        return;
    }

    if (t->value.type == HVAL_TAGVALUE) {
        printf("[HttpHeader]: name=%s type=TAGVAL [tag=%s val=%s]\n", t->name, t->value.tag_value.tag,
               t->value.tag_value.value);
    } else if (t->value.type == HVAL_LIST) {
        printf("[HttpHeader]: name=%s type=NEST_TREE: %p\n", t->name, t->value.nested_list);
        _dump_nested_list(t->value.nested_list);
    } else if (t->value.type == UNSET) {
        printf("[HttpHeader]: name=%s type=UNSET val=%s\n", t->name);
    } else {
        printf("[HttpHeader]: name=%s type=%i val=%s\n", t->name, t->value.type, t->value.single_value);
    }
}

void _dump_queryparam(queryparam *t) {
    if (t != NULL) {
        if (t->type == QUERY_VALUE) {
            printf("[QueryParam]: %s=%s\n", t->tag_name, t->value.value);
        } else if (t->type == QUERY_VALUE_LIST) {
            printf("[QueryParam]: %s=", t->tag_name);
            for (size_t i = 0; i < t->value.vec.size; i++) {
                char *curv = *(char **) vector_at(&t->value.vec, i);
                printf("%s,", curv);
            }
            printf("\n");
        }
    }
}

void httprequest_dump(httprequest *req) {
    if (req != NULL) {
        char *m;
        switch (req->method) {
            case HTTP_POST:
                m = "POST";
                break;
            case HTTP_DELETE:
                m = "DELETE";
                break;
            case HTTP_PUT:
                m = "PUT";
                break;
            case HTTP_GET:
                m = "GET";
                break;
            case HTTP_METHOD_UNDF:
                m = "UNDF";
                break;

            default:
                break;
        }


        printf("DUMP.request:\nPath=%s\nMethod=%s\nVersion: HTTP/%i.%i\n", req->path->value, m, req->version.seg1,
               req->version.seg2);
        for (size_t i = 0; i < req->httpheaders->size; i++) {
            httpheader_dump(*(httpheader **) vector_at(req->httpheaders, i));
        }

        for (size_t i = 0; i < req->queryparams->size; i++) {
            _dump_queryparam(*(queryparam **) vector_at(req->queryparams, i));
        }

        /*
            Path
            Method
            Version
            --Headers
            --QueryParams
        */
    }
}

void httprequest_dstr(httprequest *req) {
    printf("httprequest_dstr\n");
    assert(req != NULL);
    _dstr_path(req->path);
    _dstr_query_params(req->queryparams);
    free(req->queryparams);
    _dstr_headers(req->httpheaders);
    free(req->httpheaders);
}

void _dstr_path(httppath_segment *head) {
    httppath_segment *seg = head;
    while (seg != NULL) {
        printf("seg: %p %p %p\n", seg, seg->next, seg->value);
        httppath_segment *next = seg->next;
        printf("free.[path]: %s\n", seg->value);
        free(seg->value);
        free(seg);
        seg = next;
    }
}

void _dstr_query_params(vector *v) {
    for (size_t i = 0; i < v->size; i++) {
        queryparam *param = *(queryparam **) vector_at(v, i);

        if (param->type == QUERY_VALUE) {
            printf("free.[QueryParam]: tag=%s val=%s\n", param->tag_name, param->value.value);
            free(param->tag_name);
            free(param->value.value);
        } else if (param->type == QUERY_VALUE_LIST) {
            free(param->tag_name);
            for (size_t k = 0; k < param->value.vec.size; k++) {
                char **st = vector_at(&param->value.vec, k);
                printf("free.[QueryParam]: tag=%s val=%s\n", param->tag_name, *st);
                if (st != NULL) {
                    free(*st);
                }

                //free(st);
                printf("vec=size=%i\n", param->value.vec.size);
                vector_dstr(&param->value.vec);
            }
        }

        free(param);
    }

    vector_dstr(v);
}

void _dstr_headers(vector *v) {
    if (v != NULL) {
        for (size_t i = 0; i < v->size; i++) {
            httpheader *header = *(httpheader **) vector_at(v, i);
            switch (header->value.type) {
                case HVAL_WORD:
                case HVAL_VERSION:
                case HVAL_FLOAT:
                case HVAL_INT:
                case HVAL_IPADDR:
                case HVAL_PATH:
                case HVAL_SERVER_ADDR:
                case HVAL_STRING:
                    printf("free.[HttpHeader(SINGLE)] header=%s val=%s\n", header->name, header->value.single_value);
                    free(header->name);
                    free(header->value.single_value);
                    free(header);
                    break;

                case HVAL_TAGVALUE:
                    printf("free.[HttpHeader(TAGVALUE)] header=%s (tag=%s val=%s)\n", header->name,
                           header->value.tag_value.tag, header->value.tag_value.value);
                    free(header->name);
                    free(header->value.tag_value.tag);
                    free(header->value.tag_value.value);
                    free(header);
                    break;

                case HVAL_LIST:
                    _dstr_header_value_deep(header->value.nested_list);
                    free(header->name);
                    free(header);
                    break;
                default:
                    printf("DEFA\n");
                    printf("REF__=%p -> \n", header);
                    //free();
                    break;
            }
        }

        vector_dstr(v);
    }
}

void _dstr_header_value_deep(tree *tr) {
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
            header_value *val = *(header_value **) head->value;

            if (val->type == HVAL_LIST) {
                _dstr_header_value_deep(val->nested_list);
                //tree_dstr(val->nested_list);
            } else if (val->type != HVAL_TAGVALUE) {
                printf("free.[cur_head] type=%i VAL=%s\n", val->type, val->single_value);
                free(val->single_value);
            } else if (val->type == HVAL_TAGVALUE) {
                printf("free.[cur_head] type=%i (TAGVALUE)\n", val->type);
                free(val->tag_value.tag);
                free(val->tag_value.value);
            }

            free(val);
        }

        for (size_t i = 0; i < head->child_nodes.size; i++) {
            tree_node **find = (tree_node **) vector_at(&head->child_nodes, i);
            if (find != NULL) {
                queue_push(&q, find); //
            }
        }

        //printf("free.head=%p\n", head);
        free(head->value);
        vector_dstr(&head->child_nodes);
        free(head);
    }

    printf("free.tree(global)\n");
    free(tr);
    queue_clear(&q);
}
