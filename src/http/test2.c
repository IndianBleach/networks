#define _GNU_SOURCE

#include "../include/http/parser.h"
#include "../include/http/request.h"
#include "../include/sockets/epoll.h"
#include "../include/sockets/epollfd.h"
#include "../include/sockets/ntconfig.h"
#include "../include/sockets/ntsocket.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector.h>

// tokens
typedef struct parse_context parse_context;

struct parse_context {
    int cursor;
    char *buff;
    int end;
};

typedef struct httpreq_token httpreq_token;

struct httpreq_token {
    httpreq_tokentype type;
    const char *value;
    httpreq_token *next;
};

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

// fix: out format? vector/headers struct
void parse(httpreq_buff *buff) {
    parse_context ctx;
    ctx.buff = buff->ptr;
    ctx.cursor = 0;
    ctx.end = strlen(buff->ptr);

    printf("parse: cursr=%i end=%i\n", ctx.cursor, ctx.end);

    httpreq_token *head = token_new(NULL, _parse_begin);
    httpreq_token *tail = head;

    // enum stack
    bool enum_open = false;

    while (ctx.cursor < ctx.end) {
        // parsing deep enums by priority
        int enum_deep = -1;
        int enum_level = 0;
        while ((enum_deep = is_enum_value(&ctx, enum_deep)) > 0) {
            tail->next = token_new_at(enum_value_begin, &ctx.buff[enum_deep], 1);
            tail = tail->next;
            token_dump(tail);
            enum_level++;
        }

        int base_len;

        printf("cur=%c\n", ctx.buff[ctx.cursor]);

        if ((base_len = get_word(&ctx)) > 0) {
            // variants
            int local_len = 0;

            // getPath
            if ((local_len = getm_path(&ctx)) > 0) {
                printf("get_path=%i curs=%i\n", local_len, ctx.cursor);
                tail->next = token_new_at(word_path, &ctx.buff[ctx.cursor], local_len);
                tail = tail->next;
                ctx_move(&ctx, local_len);
                token_dump(tail);
            } else if ((local_len = get_tag(&ctx)) > 0) {
                printf("get_tag=%i curs=%i\n", local_len, ctx.cursor);
                tail->next = token_new_at(header_tag, &ctx.buff[ctx.cursor], local_len);
                tail = tail->next;
                ctx_move(&ctx, local_len);
                token_dump(tail);
            } else {
                printf("get_word=%i curs=%i\n", local_len, ctx.cursor);
                tail->next = token_new_at(word, &ctx.buff[ctx.cursor], base_len);
                tail = tail->next;
                ctx_move(&ctx, base_len);
                token_dump(tail);
            }

            // getLongWord
        } else if ((base_len = get_string(&ctx)) > 0) {
            // variants

            printf("get_string=%i\n", base_len);
            tail->next = token_new_at(string, &ctx.buff[ctx.cursor], base_len);
            tail = tail->next;
            ctx_move(&ctx, base_len);
            token_dump(tail);
        } else if ((base_len = get_number(&ctx)) > 0) {
            int local;
            if ((local = get_ipaddr(&ctx)) > 0) {
                printf("get_number.get_ipaddr=%i\n", local);
                tail->next = token_new_at(ipaddr, &ctx.buff[ctx.cursor], local);
                tail = tail->next;
                ctx_move(&ctx, local);
                token_dump(tail);
            } else if ((local = get_version(&ctx)) > 0) {
                printf("get_number.get_version=%i\n", local);
                tail->next = token_new_at(version, &ctx.buff[ctx.cursor], local);
                tail = tail->next;
                ctx_move(&ctx, local);
                token_dump(tail);
            } else {
                int floated;
                if ((floated = is_float(&ctx, base_len)) > 0) {
                    printf("get_number.floated=%i\n", base_len);
                } else {
                    printf("get_number.int=%i\n", base_len);
                }

                tail->next = token_new_at(number, &ctx.buff[ctx.cursor], base_len);
                tail = tail->next;
                ctx_move(&ctx, base_len);
                token_dump(tail);
            }
        }

        // symbol parsing
        if (ctx.buff[ctx.cursor] == ',') {
            if (enum_level >= 1) {
                tail->next = token_new(NULL, enum_delim_comma);
                tail = tail->next;
                ctx_move(&ctx, 1);
            }
        } else if (ctx.buff[ctx.cursor] == ';') {
            if (enum_level >= 1) {
                tail->next = token_new(NULL, enum_delim_semicolon);
                tail = tail->next;
                ctx_move(&ctx, 1);
            }
        }

        // longWord
        // sp

        ctx.cursor++;
    }
}

// TODO:
/*
    ----------------------
            HTTP REQUEST
    httprequest
        -vector<httpheader_value> headers;
        -long_string body
        -vector<httpquery_param> params; query_parser
        -char* path; look(tag=path)
        -httpmethod method;  look(tag=method)
        -ipadrr addr; look(tag=host)

    httpheader_value
    -m_value_type;
    -char name;
    -type type
    -char* single_value;
    -vector<httpheader_value*> values;

    void make_headers(httptoken_list, vector<http_header>*)

    -----------------------
            FILE-CONTROLLER-VIEW
    -определить роуты? по файлу
    -смотреть на путь и возвращать html страницу, если такая определена, подгружать зависимости
    -как отправлять файлы
    -----------------------
    HTTP RESPONSE
    -file/text
    -metadata
    -status_codes
    -type
    
    -----------------------
            CLI
    commands

    -----------------------
            CACHE
    sessions/ connections/ ssl?



*/

// utils


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
                return size;
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

int get_word(parse_context *ctx) {
    int i = ctx->cursor;
    int size = 0;
    while (i < ctx->end) {
        char p = ctx->buff[i];
        //printf("it=%c i=%i size=%i \n ", p, i, size);
        if (p >= 'a' && p <= 'z') {
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

char ctx_at(parse_context *ctx, int pos) {
    if (pos > ctx->end) {
        return ctx->buff[ctx->end];
    }

    return ctx->buff[pos];
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

// [32.1] [32] [123.0.4.2].<other symbol>
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
        printf("ip_len=%i\n", ip_len);
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
        } else {
            if (count > 1) {
                break;
            } else {
                ctx_move(ctx, -total_len);
                return -1;
            }
        }
    }

    ctx_move(ctx, -total_len); // path parsed, but cursor moving back
    return total_len;
}

// return word length
// con: -> 3
int get_tag(parse_context *ctx) {
    int word_len = 0;
    word_len = get_word(ctx);
    if (word_len < 0)
        return -1;

    char next = ctx_at(ctx, ctx->cursor + word_len);
    if (next == ':') {
        return word_len;
    } else {
        return -1;
    }
}

int main2() {
    printf("test2.start\n");

    const char *t = "connection: 12.2.344.4:2\nhost:keep-alive 123.3.4 123 123.4\0";
    /*
    parse_context ctx;
    ctx.buff = t;
    ctx.cursor = 0;
    ctx.end = strlen(t);

    int t1 = get_ipaddr(&ctx);

    printf("RES=%i\n", t1);
    return 0;
*/
    httpreq_buff *reqbuff = reqbuff_new(1024);

    reqbuff_copy(t, reqbuff);


    parse(reqbuff);

    reqbuff_dstr(reqbuff);

    printf("test2.end\n");
}