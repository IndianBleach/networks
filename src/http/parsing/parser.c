#include "../include/http/parser.h"

#include "../include/core/core.h"
#include "../include/core/coredef.h"
#include "../include/http/httpinfo.h"
#include "../include/http/request.h"

#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TOKEN
httpreq_token *token_new(const char *value, httpreq_tokentype type) {
    httpreq_token *ptr = (httpreq_token *) malloc(sizeof(httpreq_token));
    ptr->value = value;
    ptr->type = type;
    return ptr;
}

httpreq_token *token_new_at(httpreq_tokentype type, char *buff, int len) {
    httpreq_token *ptr = (httpreq_token *) malloc(sizeof(httpreq_token));

    char *val = (char *) malloc(sizeof(char) * (len + 1));
    val[len] = '\0';
    strncpy(val, buff, len);

    ptr->value = (const char *) val;
    ptr->type = type;
    return ptr;
}

void token_dump(httpreq_token *tk) {
    printf("---token_dump:");
    printf("val=%s type=%i\n", tk->value, tk->type);
}

// EXTRACT DATA
httpmethod extract_method(parse_context *ctx) {
    int cur = ctx->cursor;
    char wordbuff[24];
    int end = ctx->end;
    char *begin = &ctx->buff[cur];

    int i = cur;
    for (; i < end; i++) {
        if (begin[i] == ' ') {
            break;
        } else {
            wordbuff[i] = begin[i];
        }
    }

    wordbuff[i] = '\0';

    if (strcmp(wordbuff, "GET") == 0) {
        ctx_move(ctx, 3);
        return HTTP_GET;
    } else if (strcmp(wordbuff, "POST") == 0) {
        ctx_move(ctx, 4);
        return HTTP_POST;
    } else if (strcmp(wordbuff, "DELETE") == 0) {
        ctx_move(ctx, 7);
        return HTTP_DELETE;
    } else if (strcmp(wordbuff, "PUT") == 0) {
        ctx_move(ctx, 3);
        return HTTP_PUT;
    } else
        return HTTP_METHOD_UNDF;
};

int extract_path(parse_context *ctx, httppath_segment **__out_pathhead) {
    int cur = ctx->cursor;
    int end = ctx->end;
    char *begin = ctx->buff;
    int size = 0;
    *__out_pathhead = NULL;
    httppath_segment *tail;

    int word_len = 0;
    while (cur < end) {
        if (begin[cur] == '/') {
            cur++;
            size++;
            ctx_move(ctx, 1);
            continue;
        }

        if (begin[cur] == ' ') {
            break;
        }

        if ((word_len = get_word_ext(ctx)) > 0) {
            // head.append
            if (!(*__out_pathhead)) {
                //printf("P=%c\n", begin[cur]);
                *__out_pathhead = pathsegment_new(word_len, &begin[cur]);
                //printf("PATH_SEG(AFT)=%s\n", head->value);
                tail = *__out_pathhead;
            } else {
                tail->next = pathsegment_new(word_len, &begin[cur]);
                tail = tail->next;
            }

            ctx_move(ctx, word_len);
            cur += word_len;
            size += word_len;

        } else
            break;
    }

    return size;
};

// [1.0], [2.0]
int extract_httpversion(parse_context *ctx, httpversion *__out_version) {
    int len = get_number(ctx);
    if (len == 3) {
        int curs = ctx->cursor;
        __out_version->seg1 = (unsigned short) (ctx->buff[curs] - '0');
        __out_version->seg2 = (unsigned short) (ctx->buff[curs + 2] - '0');

        return len;
    }

    return -1;
}

// [?name=john...], [name=john...]
int extract_queryparams(parse_context *ctx, vector *__out_vec_qparams) {
    if (!__out_vec_qparams)
        return -1;

    int cur = ctx->cursor;
    int end = ctx->end;
    //int size = 0;
    char *begin = ctx->buff;

    if (ctx_at(ctx, ctx->cursor) == '?') {
        cur++;
        ctx_move(ctx, 1);
    }

    int tag_len = 0;
    int val_len = 0;

    printf("parsing=%s\n", ctx->buff);

    // parsing
    void *iter_begin = __out_vec_qparams->iterator.begin;

    // create hashmap
    hashmap map;
    hashmap_init(&map, sizeof(queryparam *));

    while (cur < end) {
        if ((tag_len = get_tag(ctx, '=')) > 0) {
            char *tag_name = cstr_new(tag_len, &ctx->buff[ctx->cursor]);
            ctx_move(ctx, tag_len + 1); // +1 for :
            cur += tag_len;

            if ((val_len = get_querytag_value(ctx)) > 0) {
                char *tag_value = cstr_new(val_len, &ctx->buff[ctx->cursor]);

                // check if TAG EXISTS
                // get the value, add new node. (its LIST_VALUE - ARRAY)
                queryparam **find = (queryparam **) hashmap_get(&map, tag_name);

                if (find != NULL) {
                    // free temp values
                    free(tag_name);

                    queryparam *elem = *find;
                    if (elem->type == QUERY_VALUE) {
                        // switch from SINGLE_VALUE to LIST_VALUE, copy old SINGLE_VALUE to list.
                        queryparam_switch_on_list(elem, QUERY_VALUE_LIST);
                        // add new value
                        vector_pushback(&elem->value.vec, &(tag_value));
                        vector_dump(&elem->value.vec);
                    } else {
                        // add value to existing list;
                        vector_pushback(&elem->value.vec, &(tag_value));
                    }
                } else {
                    // add to vector
                    // add to map
                    printf("node.single: [%s]=%s\n", tag_name, tag_value);
                    queryparam *qp = malloc(sizeof(queryparam));
                    queryparam_init(qp, QUERY_VALUE, tag_name, tag_value);

                    hashmap_add(&map, tag_name, &(qp)); // REF ON BUFFER


                    vector_pushback(__out_vec_qparams, &(qp)); // add ref on query_param
                }


                cur += val_len;
                ctx_move(ctx, val_len); // +1 for delimiter
            }
        }

        ctx_move(ctx, 1);
        cur++;
    }

    hashmap_dstr(&map);

    return 1;
}

/////////////// parsing utils.
char *ctx_now(parse_context *ctx) { return &ctx->buff[ctx->cursor]; }

unsigned int _get_list_deep(parse_context *ctx) {
    int cur = ctx->cursor;
    int end = ctx->end;
    char *begin = ctx->buff;

    int list_deep_level = 0;
    while (cur < end) {
        //printf("CUR=%i P=%c\n", cur, begin[cur]);

        //printf("vvv=%c\n", begin[cur]);
        if (begin[cur] == '\n') // tag value line end
            break;
        if (begin[cur] == ':') // next tag
            break;

        if (begin[cur] == ',') {
            list_deep_level = 1;
        } else if (begin[cur] == ';') {
            list_deep_level = 2;
            break;
        }
        cur++;
    }

    //printf("VVVV=%i\n", list_deep_level);

    return list_deep_level;
};

int _nested_value_deep_len(parse_context *ctx) {}

void _header_set_singval(httpheader *header, char *val) { header->value.single_value = val; }

void _header_add_singval(httpheader *header, char **dptr_val, tree_node *nested_list) {
    printf("_header_add_singval=%i\n", nested_list->child_nodes.size);
    vector_pushback(&nested_list->child_nodes, dptr_val);
    tree_dump(header->value.nested_list);
}

header_value *_headervalue_new_single(header_value_type type, const char *dptr_val) {
    header_value *val = (header_value *) malloc(sizeof(header_value));
    val->type = type;
    val->single_value = dptr_val;

    return val;
}

header_value *_headervalue_new_tagvalue(const char *tagname, const char *tagvalue) {
    header_value *val = (header_value *) malloc(sizeof(header_value));
    val->type = HVAL_TAGVALUE;
    val->tag_value.tag = tagname;
    val->tag_value.value = tagvalue;

    return val;
}

header_value *_headervalue_new_list() {
    header_value *val = (header_value *) malloc(sizeof(header_value));
    val->type = HVAL_LIST;
    val->single_value = NULL;
    tree *tr = (tree *) malloc(sizeof(tree));
    tree_init(tr, sizeof(header_value *));
    val->nested_list = tr;
    return val;
}

void parse(httprequest_buff *buff) {
    parse_context ctx;
    ctx.buff = buff->ptr;
    ctx.cursor = 0;
    ctx.end = strlen(buff->ptr);

    printf("PARSING: %s\n", buff->ptr);

    //printf("parse: cursr=%i end=%i\n", ctx.cursor, ctx.end);

    vector outv;
    vector_init(&outv, 10, sizeof(httpheader *));

    // fix?
    bool value_open = false;
    header_value_type value_type = -1;
    bool tag_open = false;


    // CUR_HEADER
    httpheader *cur_header = NULL;
    unsigned int cur_list_deep = 0;
    tree_node *cur_nested_list_node = NULL;

    while (ctx.cursor < ctx.end) {
        // parsing deep enums by priority
        //printf("___CURS=%i\n", ctx.cursor);
        int base_len;
        if ((base_len = get_tag(&ctx, ':')) > 0) {
            char *tagname = substr(ctx_now(&ctx), base_len);
            //printf("__TAGNAME=%s\n", tagname);

            // ADD OLD BUILDER HEADER
            if (cur_header != NULL) {
                printf("[added] %p\n", cur_header);
                //httpheader_dump(cur_header);
                vector_pushback(&outv, &(cur_header));
            }


            tag_open = true;
            value_open = true;

            ctx_move(&ctx, base_len + 1); // tag + :

            cur_list_deep = _get_list_deep(&ctx);
            cur_header = httpheader_new(value_type);
            cur_header->name = tagname;
            if (cur_list_deep == 0) {
            }
            if (cur_list_deep == 1) {
                tree *nested = (tree *) malloc(sizeof(tree));
                tree_init(nested, sizeof(header_value *));
                cur_header->value.nested_list = nested;
                cur_header->value.type = HVAL_LIST;
                cur_nested_list_node = nested->head;
            } else if (cur_list_deep == 2) {
                // FIRST
                tree *n1 = (tree *) malloc(sizeof(tree));
                tree_init(n1, sizeof(header_value *));
                cur_header->value.nested_list = n1;
                cur_header->value.type = HVAL_LIST;
                // SEC
                tree *n2 = (tree *) malloc(sizeof(tree));
                tree_init(n2, sizeof(header_value *));
                header_value *v2 = (header_value *) malloc(sizeof(header_value));
                v2->type = HVAL_LIST;
                v2->nested_list = n2;
                cur_nested_list_node = n2->head;
                tree_add(cur_header->value.nested_list, cur_header->value.nested_list->head,
                         &(v2)); // add 2 level top top level
                //printf("d1=%p d2=%p\n", cur_header->value.nested_list->head, cur_nested_list_node);
            }

            //ctx_move(&ctx, base_len + 1);
        }

        //printf("base_len=%i curs=%i\n", base_len, ctx.cursor);

        if ((base_len = get_fulladdr(&ctx)) > 0) {
            char *parse_value = substr(ctx_now(&ctx), base_len);

            // add header
            if (cur_list_deep > 0) {
                header_value *v1 = _headervalue_new_single(HVAL_SERVER_ADDR, parse_value);
                tree_add(cur_header->value.nested_list, cur_nested_list_node, &(v1));
            } else if (cur_list_deep == 0) {
                cur_header->value.type = HVAL_SERVER_ADDR;
                cur_header->value.single_value = parse_value;
            }

            ctx_move(&ctx, base_len);

        }
        // tagvalue
        else if ((base_len = get_word(&ctx)) > 0) {
            //printf("---->get_word\n");
            // variants
            int local_len = 0;
            char *_tagval_value = NULL;
            char *_tagval_name = NULL;

            if ((local_len = getm_path(&ctx)) > 0) {
                char *path = substr(ctx_now(&ctx), local_len);

                // add header
                if (cur_list_deep > 0) {
                    header_value *v1 = _headervalue_new_single(HVAL_PATH, path);
                    tree_add(cur_header->value.nested_list, cur_nested_list_node, &(v1));
                } else if (cur_list_deep == 0) {
                    cur_header->value.type = HVAL_PATH;
                    cur_header->value.single_value = path;
                }

                // moving
                ctx_move(&ctx, local_len);

            } else if ((local_len = get_tag(&ctx, ':')) > 0) {
                // break current iteration. top level parsing tag.
                continue;
            } else if ((local_len = get_tagvalue(&ctx, &(_tagval_value), &(_tagval_name))) > 0) {
                // add TAGVALUE header
                if (cur_list_deep > 0) {
                    header_value *v1 = _headervalue_new_tagvalue(_tagval_name, _tagval_value);
                    tree_add(cur_header->value.nested_list, cur_nested_list_node, &(v1));
                } else if (cur_list_deep == 0) {
                    cur_header->value.type = HVAL_TAGVALUE;
                    cur_header->value.tag_value.tag = _tagval_name;
                    cur_header->value.tag_value.value = _tagval_value;
                }

                ctx_move(&ctx, local_len);
            } else {
                char *word = substr(ctx_now(&ctx), base_len);

                // add header
                if (cur_list_deep > 0) {
                    header_value *v1 = _headervalue_new_single(HVAL_WORD, word);
                    tree_add(cur_header->value.nested_list, cur_nested_list_node, &(v1));
                } else if (cur_list_deep == 0) {
                    cur_header->value.type = HVAL_WORD;
                    cur_header->value.single_value = word;
                }

                ctx_move(&ctx, base_len);
                //httpheader_dump(cur_header);
            }

            // getLongWord
        } else if ((base_len = get_string(&ctx)) > 0) {
            char *string = substr(ctx_now(&ctx), base_len);

            // add header
            if (cur_list_deep > 0) {
                header_value *v1 = _headervalue_new_single(HVAL_STRING, string);
                tree_add(cur_header->value.nested_list, cur_nested_list_node, &(v1));
            } else if (cur_list_deep == 0) {
                cur_header->value.type = HVAL_STRING;
                cur_header->value.single_value = string;
            }
            // moving
            ctx_move(&ctx, base_len);

        } else if ((base_len = get_number(&ctx)) > 0) {
            //printf("---->get_number\n");
            int local;
            if ((local = get_ipaddr(&ctx)) > 0) {
                char *ipaddr = substr(ctx_now(&ctx), local);
                //printf("IPADDR=%s\n", ipaddr);
                // add header
                if (cur_list_deep > 0) {
                    header_value *v1 = _headervalue_new_single(HVAL_IPADDR, ipaddr);
                    tree_add(cur_header->value.nested_list, cur_nested_list_node, &(v1));
                } else if (cur_list_deep == 0) {
                    cur_header->value.type = HVAL_IPADDR;
                    cur_header->value.single_value = ipaddr;
                }
                // moving
                ctx_move(&ctx, local);

            } else if ((local = get_version(&ctx)) > 0) {
                //printf("---->get_number.get_version\n");

                char *parse_value = substr(ctx_now(&ctx), local);
                //printf("---VAL=%s\n", parse_value);
                // add header
                if (cur_list_deep > 0) {
                    header_value *v1 = _headervalue_new_single(HVAL_VERSION, parse_value);
                    tree_add(cur_header->value.nested_list, cur_nested_list_node, &(v1));
                } else if (cur_list_deep == 0) {
                    cur_header->value.type = HVAL_VERSION;
                    cur_header->value.single_value = parse_value;
                }

                // moving
                ctx_move(&ctx, local);

            } else {
                header_value_type parse_value_type;
                if (is_float(&ctx, base_len) > 0) {
                    parse_value_type = HVAL_FLOAT;
                } else {
                    parse_value_type = HVAL_INT;
                }

                char *parse_value = substr(ctx_now(&ctx), base_len);
                //printf("---VAL=%s\n", parse_value);
                // add header
                if (cur_list_deep > 0) {
                    header_value *v1 = _headervalue_new_single(parse_value_type, parse_value);
                    tree_add(cur_header->value.nested_list, cur_nested_list_node, &(v1));
                } else if (cur_list_deep == 0) {
                    cur_header->value.type = parse_value_type;
                    cur_header->value.single_value = parse_value;
                }

                ctx_move(&ctx, base_len);
            }
        }

        //printf("cur=%c\n", ctx.buff[ctx.cursor]);

        // symbol parsing
        if (ctx.buff[ctx.cursor] == ',') {
            //pass;
        } else if (ctx.buff[ctx.cursor] == ';') {
            printf("SEMICOL\n");
            // ADD NEW TOP LEVEL LIST IN TREE
            // new header (LIST) add to top level (to head child nodes).
            // cur_actual_list = newlist
            header_value *v = _headervalue_new_list();
            tree_add(cur_header->value.nested_list, cur_header->value.nested_list->head, &(v));
            cur_nested_list_node = v->nested_list->head;
        }

        // longWord
        // sp

        ctx.cursor++;
    }

    // add last element
    if (cur_header != NULL) {
        printf("[added2] %p\n", cur_header);
        //httpheader_dump(cur_header);
        vector_pushback(&outv, &(cur_header));
    }


    // for vec
    // -> dump header

    for (size_t i = 0; i < outv.size; i++) {
        httpheader_dump(*(httpheader **) vector_at(&outv, i));
    }

    printf("END\n");
}

char ctx_at(parse_context *ctx, int pos) {
    if (pos > ctx->end) {
        return ctx->buff[ctx->end];
    }

    return ctx->buff[pos];
}

void ctx_move(parse_context *ctx, int steps) {
    int old = ctx->cursor;
    ctx->cursor += steps;
    if (ctx->cursor < 0)
        ctx->cursor = 0;
    else if (ctx->cursor > ctx->end) {
        ctx->cursor = ctx->end;
    }
    //printf("ctx_move: %i -> %i\n", old, ctx->cursor);
}

// q=0.9 q="string" q=word
int get_tagvalue(parse_context *ctx, char **__out_value, char **__out_tagname) {
    char *buff = &ctx->buff[ctx->cursor];
    int old_curs = ctx->cursor;
    int end = ctx->end;
    int i = 0;
    int word_len = get_word(ctx);
    if (word_len > 0) {
        if (buff[word_len] == '=') {
            int local_len;

            ctx_move(ctx, word_len + 1);

            if ((local_len = get_string(ctx)) > 0) {
                char *tag = substr(buff, word_len);
                char *val = substr(&buff[word_len + 1], local_len);
                *__out_tagname = tag;
                *__out_value = val;

                ctx->cursor = old_curs; //moving back
                return word_len + local_len + 1;
            } else if ((local_len = get_number(ctx)) > 0) {
                char *tag = substr(buff, word_len);
                char *val = substr(&buff[word_len + 1], local_len);
                *__out_tagname = tag;
                *__out_value = val;

                ctx->cursor = old_curs; //moving back

                return word_len + local_len + 1;
            } else if ((local_len = get_word(ctx)) > 0) {
                char *tag = substr(buff, word_len);
                char *val = substr(&buff[word_len + 1], local_len);
                *__out_tagname = tag;
                *__out_value = val;

                ctx->cursor = old_curs; //moving back
                return word_len + local_len + 1;
            }
        }
    }

    ctx->cursor = old_curs;

    return -1;
}

// https://127.0.0.1:8013 http://127.0.0.1:8013
int get_fulladdr(parse_context *ctx) {
    char *buff = &ctx->buff[ctx->cursor];
    int old_curs = ctx->cursor;
    int end = ctx->end;
    int i = 0;
    int word_len = get_word(ctx);
    if (word_len > 0) {
        if (word_len + 3 >= end) {
            return -1;
        }

        char *val = substr(buff, word_len);
        if (strcmp(val, "https") != 0 && strcmp(val, "http") != 0) {
            free(val);
            return -1;
        }

        if (buff[word_len] == ':' && buff[word_len + 1] == '/' && buff[word_len + 2] == '/') {
            ctx_move(ctx, word_len + 3);
            int ip_len = get_ipaddr(ctx);
            if (ip_len > 0) {
                ctx->cursor = old_curs; // back cursor
                return word_len + 3 + ip_len;
            }
        }
    }

    ctx->cursor = old_curs; // back cursor

    return -1;
}

// return len("len(word)")
// "win" -> 5
int get_string(parse_context *ctx) {
    int local = ctx->cursor;
    int size = 0;
    if (ctx->buff[local] == '"') {
        // read until end tag
        local++;
        size++;

        while (local < ctx->end) {
            if (ctx->buff[local] == '"') {
                return size + 1;
            }

            if (ctx->buff[local] == '\n') {
                return -1;
            }

            local++;
            size++;
        }
    }

    return -1;
}

int is_enum_value(parse_context *ctx, int local_end) {
    int i = ctx->cursor;

    int comma = 0;
    int semicolon = 0;

    if (local_end <= 0) {
        local_end = ctx->end;
    }

    while (ctx->buff[i] != '\0' && i < local_end) {
        switch (ctx->buff[i]) {
            case ',':
                comma = i;
                break;
            case ';':
                semicolon = i;
                break;
            default:
                break;
        }

        if (ctx->buff[i] == '\n') {
            break;
        }

        i++;
    }

    if (semicolon > 0)
        return semicolon;
    if (comma > 0)
        return comma;

    return -1;
}

int is_float(parse_context *ctx, int len) {
    int i = 0;
    char *begin = &ctx->buff[ctx->cursor];
    int end = len;
    while (i < end) {
        if (begin[i] == '.') {
            return len;
        } else {
            i++;
        }
    }

    return -1;
}

int get_word(parse_context *ctx) {
    int i = ctx->cursor;
    int size = 0;
    while (i < ctx->end) {
        char p = ctx->buff[i];
        //printf("it=%c i=%i size=%i \n ", p, i, size);
        if (isalpha(p)) {
            size++;
        } else if (p >= '0' && p <= '9') {
            if (size == 0)
                return -1; // begin from number, is now word
            size++;
        } else if (p == '-') {
            if (size == 0)
                return -1; // begin from -, is now word
            size++;
        } else if (p == '+') {
            if (size == 0)
                return -1; // begin from +, is now word
            size++;
        } else if (p == '_') {
            size++;
        } else {
            break;
        }

        //printf("get_word=%c\n", p);

        if (ctx->buff[i] == '\n') {
            return size;
        }

        i++;
    }

    return size;
}

int get_word_ext(parse_context *ctx) {
    int i = ctx->cursor;
    char *begin = ctx->buff;
    int size = 0;

    int word_len = get_word(ctx);
    if (word_len > 0) {
        //printf("get_word_ext.get_word=%i\n", word_len);
        //printf("get_word_ext.cur=%c\n", begin[word_len]);
        //ctx->cursor += word_len;
        i += word_len; // step next
        //printf("CUR=%c\n", begin[i]);
        if (begin[i] == '.') {
            //ctx->cursor++;
            i++;
            int ext_len = get_word(ctx);
            if (ext_len > 0) {
                //printf("get_word_ext.get_wordEXT=%i\n", ext_len);
                return word_len + ext_len;
            }
        }
    }

    return word_len;
}

int get_word_version(parse_context *ctx) {
    int i = ctx->cursor;
    char *begin = ctx->buff;
    int size = 0;

    int word_len = get_word(ctx);
    if (word_len > 0) {
        i += word_len; // step next
        //printf("CUR=%c\n", begin[i]);
        if (begin[i] == '/') {
            //ctx->cursor++;
            i++;
            int vers_len = get_version(ctx);
            if (vers_len > 0) {
                //printf("get_word_ext.get_wordEXT=%i\n", ext_len);
                return word_len + vers_len;
            }
        }
    }

    return word_len;
}

int get_querytag_value(parse_context *ctx) {
    int cur = ctx->cursor;
    int end = ctx->end;
    char *begin = ctx->buff;

    int size = 0;

    while (cur < end) {
        if (begin[cur] == '\0' || begin[cur] == '\n' || begin[cur] == ' ' || begin[cur] == '&') {
            break;
        } else {
            size++;
            cur++;
        }
    }

    if (size == 0)
        return -1;

    return size;
}

// [32.1] [32] [123.0].4.2.<other symbol>
// [17.2].3
int get_number(parse_context *ctx) {
    int local = ctx->cursor;
    int last_seg = 0;
    int size = 0;

    while (local < ctx->end && ctx->buff[local] >= '0' && ctx->buff[local] <= '9') {
        local++;
        size++;
        if (local < ctx->end && ctx->buff[local] == '.') {
            if (last_seg != 0) {
                break;
            }
            local++;
            size++;

            last_seg = local;
        }
    }

    if (size > 0) {
        return size;
    }

    return -1;
}

// [32.1.2] [123.0.4.2].<other symbol>
// [17.2.3].
int get_version(parse_context *ctx) {
    int local = ctx->cursor;
    int last_seg = 0;
    int size = 0;
    int expect_dots = 2;

    while (local < ctx->end && ctx->buff[local] >= '0' && ctx->buff[local] <= '9') {
        local++;
        size++;
        if (local < ctx->end && ctx->buff[local] == '.') {
            local++;
            size++;
            last_seg = local;
            expect_dots--;
        }
    }

    if (expect_dots > 0) {
        return -1;
    }

    if (last_seg > 0) {
        //char next = ctx->buff[last_seg];
        char next = ctx_at(ctx, last_seg);
        //char next = ctx_at(ctx, last_seg + 1);
        //printf("NEXT=%c seg=%i\n", next, last_seg);
        if (next <= '0' || next >= '9') {
            return --size;
        }
        { return size; }
    }

    if (size > 0) {
        return size;
    }

    return -1;
}

// 123.4.56.2:4040
// 123.4.56.2:40
int get_ipaddr(parse_context *ctx) {
    int ip_len = 0;
    int curs = ctx->cursor;
    int port_len = 0;
    int end = ctx->end;
    char *begin = &ctx->buff[ctx->cursor];
    if ((ip_len = get_version(ctx)) > 0) {
        // valid address, has 4 '.'
        //printf("ip_len=%i\n", ip_len);
        int require_dot = 3;
        int segment_max_len = 3;
        for (int i = 0; i < ip_len; i++) {
            if (begin[i] == '.') {
                require_dot--;
                segment_max_len = 3;
            } else {
                segment_max_len--;
            }

            if (segment_max_len < 0) {
                //printf("segment_max_len=%i\n", require_dot);
                return -1;
            }
        }

        if (require_dot != 0) {
            //printf("require_dot=%i\n", require_dot);
            return -1;
        }

        char next = ctx_at(ctx, curs + ip_len);
        if (next == ':') {
            curs += ip_len;
            curs++;
            // can't use get_number because they take floated numbers
            // parse
            while (curs < end) {
                //printf("cur==%c\n", ctx->buff[curs]);
                if (ctx->buff[curs] >= '0' && ctx->buff[curs] <= '9') {
                    port_len++;
                } else {
                    break;
                }
                curs++;
            }

            //printf("PORT_LEN=%i\n", port_len);
            if (port_len > 0 && port_len <= 4) {
                return ip_len + port_len + 1;
            }
        }
    }

    return -1;
}

int getm_path(parse_context *ctx) {
    int word_len = 0;
    int total_len = 0;
    int count = 0;
    char next;
    while ((word_len = get_word(ctx)) > 0) {
        ctx->cursor += word_len;
        //ctx_move(ctx, word_len);
        next = ctx_at(ctx, ctx->cursor);
        total_len += word_len;
        //printf("NEXT=%c LEN=%i word_len=%i\n", next, total_len, word_len);
        if (next == '/') {
            count++;
            total_len++;
            ctx->cursor += 1;
            //ctx_move(ctx, 1);
            //printf("CTX.CUR=%c\n", ctx->buff[ctx->cursor]);
        } else if (!isalpha(next) || !isalnum(next)) {
            break;
        }
    }

    // path parsed, but cursor moving back
    ctx_move(ctx, -total_len);

    if (count == 0) {
        return -1;
    }


    return total_len;
}

// return word length
// con: -> 3
int get_tag(parse_context *ctx, char delimiter) {
    int word_len = 0;
    word_len = get_word(ctx);
    if (word_len < 0)
        return -1;

    //printf("WORD=%i\n", word_len);

    char next = ctx_at(ctx, ctx->cursor + word_len);
    char next2 = ctx_at(ctx, ctx->cursor + word_len + 1);
    if (next == delimiter && next2 != '/') {
        return word_len;
    } else {
        return -1;
    }
}
