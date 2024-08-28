#include "parser.h"
#include "token.h"

enum http_method methods_lookup[K_M_FIN] = {
    [K_GET] = GET,
    [K_POST] = POST,
    [K_PUT] = PUT,
};

int p_is_end(parser *p) { return peek()->type == END; }

token *p_move(parser *p) {
    if (is_end())
        return NULL;
    return vector_get(p->tokens, p->cursor++);
}

token *p_peek(parser *p) {
    if (is_end())
        return NULL;
    return vector_get(p->tokens, p->cursor);
}

token *p_peek_next(parser *p) {
    token *tkn = vector_get(p->tokens, p->cursor + 1);
    if (!tkn || tkn->type == END)
        return NULL;
    return tkn;
}

int match(token *tkn, int cnt, ...) {
    va_list ap;
    va_start(ap, cnt);
    for (; cnt > 0; cnt--) {
        if (tkn->type != va_arg(ap, int))
            return 0;
    }
    va_end(ap);
    return 1;
}

int is_http_method(token *tkn) { return K_M_START < tkn->type && tkn->type < K_M_FIN; }

void parse_err(const char *exp, const char *act) { printf("Expect %s, got %s\n", exp, act); }

void parser_init(parser *p, vector *tkns) {
    p->tokens = tkns;
    p->cursor = p->start = 0;
}

void parser_destroy(parser *p) { return; }

void parse_req(parser *p, http_req *sr, char *req) { parse_req_line(p, sr, &req); }

// TODO: rewrite it in better way
int parse_req_line(parser *p, http_req *sr, char **req) {
    token *tkn = move();
    if (!is_http_method(tkn)) {
        parse_err("HTTP METHOD", tkn->lexeme);
        return 1;
    }
    sr->method = methods_lookup[tkn->type];
    if (!match(move(), SPACE)) {
        parse_err("SPACE", tkn->lexeme);
        return 1;
    }
    if (match(move(), SLASH)) {
        if (match(peek(), IDENTIFIER)) {
            size_t sz = strlen(peek()->lexeme) + 1;
            char buffer[sz + 1];
            buffer[0] = '/';
            strcpy(buffer + 1, peek()->lexeme);
            buffer[sz] = '\0';
            req_fill_path(sr, buffer);
            if (!match(move(), SPACE)) {
                parse_err("SPACE", peek()->lexeme);
            }
        } else if (match(peek(), SPACE)) {
            req_fill_path(sr, "/");
            move();
        }
    } else {
        parse_err("SLASH", tkn->lexeme);
        return 1;
    }
    if (match(move(), K_HTTP)) {
        if (match(move(), SLASH)) {
            if (match(peek(), NUMBER)) {
                if (strcmp(peek()->lexeme, "1.1") == 0)
                    sr->ver = VER_1_1;
                else if (strcmp(peek()->lexeme, "2.0") == 0)
                    sr->ver = VER_2;
                move();
            } else {
                parse_err("NUMBER", "");
                return 1;
            }
        } else {
            parse_err("SLASH", "");
            return 1;
        }
    } else {
        parse_err("HTTP", "");
        return 1;
    }
    if (match(move(), CR) && match(peek(), LF))
        return 0;

    return 1;
}

int main(void) {
    return 0;
}
