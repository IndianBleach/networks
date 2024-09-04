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
        int level = 0;
        while ((enum_deep = is_enum_value(&ctx, enum_deep)) > 0) {
            tail->next = token_new_at(enum_value_begin, &ctx.buff[enum_deep], 1);
            tail = tail->next;
            token_dump(tail);
            level++;
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

            continue;
            // getTag()
            //printf("deep.pre.get_tag: curs=%i\n", ctx.cursor);

            // getPath()
            // getTag()
            // getLongWord
        }
        if ((base_len = get_string(&ctx)) > 0) {
            // variants
            // --version
            // --addr

            printf("get_string=%i\n", base_len);
            tail->next = token_new_at(string, &ctx.buff[ctx.cursor], base_len);
            tail = tail->next;
            ctx_move(&ctx, base_len);
            token_dump(tail);
            continue;
        }
        if ((base_len = get_number(&ctx)) > 0) {
            printf("get_number=%i\n", base_len);
            tail->next = token_new_at(number, &ctx.buff[ctx.cursor], base_len);
            tail = tail->next;
            ctx_move(&ctx, base_len);
            token_dump(tail);
            continue;
        }

        // isNumber=float|int

        // isSymbol
        // --description

        ctx.cursor++;
    }
}

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

// [32.1] [32] [123.0.4.2].<other symbol>
int get_number(parse_context *ctx) {
    int local = ctx->cursor;
    int last_seg = 0;
    int size = 0;

    while (local < ctx->end && ctx->buff[local] >= '0' && ctx->buff[local] <= '9') {
        local++;
        size++;
        if (local < ctx->end && ctx->buff[local] == '.') {
            local++;
            size++;
            last_seg = local;
        }
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

int main() {
    printf("test2.start\n");

    const char *t = "connection: \"long\" 17.2\0";

    httpreq_buff *reqbuff = reqbuff_new(1024);

    reqbuff_copy(t, reqbuff);


    parse(reqbuff);

    reqbuff_dstr(reqbuff);

    printf("test2.end\n");
}