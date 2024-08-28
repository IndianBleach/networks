#include "parser.h"

// enum http_method methods_lookup[K_M_FIN] = {
//     [K_GET] = GET,
//     [K_POST] = POST,
//     [K_PUT] = PUT,
// };
//

int _is_end(parser *p) {
    return p->cursor == p->tokens->len;
}

token *_move(parser *p) {
    if (is_end())
        return NULL;
    return vector_get(p->tokens, p->cursor++);
}

token *_peek(parser *p) {
    if (is_end())
        return NULL;
    return vector_get(p->tokens, p->cursor);
}

token *_peek_next(parser *p) {
    if (p->cursor >= p->tokens->len - 1)
        return NULL;
    return vector_get(p->tokens, p->cursor + 1);
}

void parser_init(parser *p, vector *tkns) {
    p->tokens = tkns;
    p->cursor = p->start = 0;
}

void parser_destroy(parser *p) {
}

void parse_req(http_req *sr, char *req) {
    parse_req_line(sr, &req);
}

int parse_req_line(http_req *sr, char **req) {
    return 0;
}

int main(void) {
    return 0;
}
